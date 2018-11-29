#include "solver.h"

/* Constructors */
Solver::Solver(const std::vector<Border>& inBorders, const std::vector<Node>& inNodes,
	const std::vector<Material>& inParticles)
{
	borders = inBorders;
	nodes = inNodes;
	particles = inParticles;

	blen = borders.size();
	ilen = nodes.size();
}



/* -----------------------------------------------------------------------
|					MATERIAL POINT METHOD ALGORITHM						 |
----------------------------------------------------------------------- */


// Transfer from Particles to Grid nodes
void Solver::P2G()				
{
	// plen is computed here for when we add particles mid-simulaion
	plen = particles.size();							

	#pragma omp parallel for 
	for (int p = 0; p < plen; p++)
	{
		// Pre-update Ap (in particle loop)
		particles[p].ConstitutiveModel();				

		// Index of bottom-left node closest to the particle
		int node_base =									
			(X_GRID + 1) * static_cast<int>(particles[p].Xp[1] - Translation_xp[1])
			+ static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

		// Loop over all the close nodes (depend on interpolation through bni)
		for (int y = bni; y < 3; y++) {					
			for (int x = bni; x < 3; x++)
			{			
				// Index of the node
				int node_id = node_base + x + (X_GRID + 1) * y;

				// Distance and weight
				Vector2f dist = particles[p].Xp - nodes[node_id].Xi;		
				float Wip = getWip(dist);
				Vector2f dWip = getdWip(dist);

				// Pre-compute node mass, node velocity and pre-update force increment (APIC)
				float inMi = Wip * particles[p].Mp;							
				Vector2f inVi = Wip * particles[p].Mp *
					(particles[p].Vp + Dp_scal * H_INV * H_INV * particles[p].Bp * (-dist));

				Vector2f inFi = particles[p].Ap * dWip;

				// Udpate mass, velocity and force 
				// (atomic operation because 2 particles (i.e threads) can have nodes in commun)
				#pragma omp atomic
				nodes[node_id].Mi += inMi;

				#pragma omp atomic
				nodes[node_id].Vi[0] += inVi[0];
				#pragma omp atomic
				nodes[node_id].Vi[1] += inVi[1];

				#pragma omp atomic
				nodes[node_id].Fi[0] += inFi[0];
				#pragma omp atomic
				nodes[node_id].Fi[1] += inFi[1];
			}
		}
	}
}


// Update node force and velocity
void Solver::UpdateNodes()
{
	// Dynamic parallelization because not all the nodes are active
	#pragma omp parallel for schedule (dynamic)
	for (int i = 0; i < ilen; i++)
	{
		if (nodes[i].Mi > 0)
		{	
			// Finish updating velocity, force, and apply updated force
			// (not done before because the loop was on particles)
			nodes[i].Vi /= nodes[i].Mi;
			nodes[i].Fi = DT * (-nodes[i].Fi / nodes[i].Mi + G);
			nodes[i].Vi += nodes[i].Fi;

			// Apply collisions and frictions
			nodes[i].NodeCollisions();
			#if FRICTION
			nodes[i].NodeFrictions();
			#else
			nodes[i].Vi_fri = nodes[i].Vi_col;
			#endif
		}
	}
}


// Transfer from Grid nodes to Particles
void Solver::G2P()				
{
	#pragma omp parallel for 
	for (int p = 0; p < plen; p++)
	{		
		// Index of bottom-left node closest to the particle
		int node_base =
			(X_GRID + 1) * static_cast<int>(particles[p].Xp[1] - Translation_xp[1])
			+ static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

		// Set velocity and velocity field to 0 for sum update
		particles[p].Vp.setZero();
		particles[p].Bp.setZero();

		// Loop over all the close nodes (depend on interpolation through bni)
		for (int y = bni; y < 3; y++) {
			for (int x = bni; x < 3; x++)
			{
				// Index of the node
				int node_id = node_base + x + (X_GRID + 1) * y;
				
				// Distance and weight
				Vector2f dist = particles[p].Xp - nodes[node_id].Xi;
				float Wip = getWip(dist);
				
				// Update velocity and velocity field (APIC)
				particles[p].Vp += Wip * nodes[node_id].Vi_fri;
				particles[p].Bp += Wip * (nodes[node_id].Vi_fri*(-dist).transpose());
			}
		}
	}
}


// Update particle deformation data and position
void Solver::UpdateParticles()
{
	#pragma omp parallel for 
	for (int p = 0; p < plen; p++)
	{
		// Index of bottom-left node closest to the particle
		int node_base =
			(X_GRID + 1) * static_cast<int>(particles[p].Xp[1] - Translation_xp[1])
			+ static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

		// Save position to compute nodes-particle distances and update position in one loop
		Vector2f Xp_buff = particles[p].Xp;
		particles[p].Xp.setZero();
		//  T ~ nodal deformation
		Matrix2f T; T.setZero();

		// Loop over all the close nodes (depend on interpolation through bni)
		for (int y = bni; y < 3; y++) {
			for (int x = bni; x < 3; x++)
			{
				// Index of the node
				int node_id = node_base + x + (X_GRID + 1) * y;

				// Distance and weight
				Vector2f dist = Xp_buff - nodes[node_id].Xi;
				float Wip = getWip(dist);
				Vector2f dWip = getdWip(dist);

				// Update position and nodal deformation
				particles[p].Xp += Wip * (nodes[node_id].Xi + DT * nodes[node_id].Vi_col);
				T += nodes[node_id].Vi_col*dWip.transpose();
			}
		}

		// Update particle deformation gradient (elasticity, plasticity etc...)
		particles[p].UpdateDeformation(T);
	}
}


// Reset active nodes data
void Solver::ResetGrid()
{
	#pragma omp parallel for schedule (dynamic)
	for (int i = 0; i < ilen; i++)
		if (nodes[i].Mi > 0)
			nodes[i].ResetNode();
}



/* -----------------------------------------------------------------------
|								  DRAWING			 					 |
----------------------------------------------------------------------- */


// Draw particles, border and nodes (if selected).
void Solver::Draw()
{
	// Draw borders
	for (size_t b = 0; b < blen; b++)
		borders[b].DrawBorder();

	// Draw nodes
	#if DRAW_NODES
	for (size_t i = 0; i < ilen; i++)
		nodes[i].DrawNode();
	#endif

	// Draw particles
	for (size_t p = 0; p < plen; p++)
		particles[p].DrawParticle();
}


// Write particle position to .ply file (used in Houdini for ex)
void Solver::WriteToFile(int frame)
{
	std::ofstream output;
	std::string fileName = "out/ply/frame_" + std::to_string(frame) + ".ply";

	output.open(fileName);
	output << "ply" << std::endl;
	output << "format ascii 1.0" << std::endl;
	output << "element vertex " << particles.size() << std::endl;
	output << "property float x" << std::endl;
	output << "property float y" << std::endl;
	output << "property float z" << std::endl;
	output << "element face 0" << std::endl;
	output << "property list uint int vertex_indices" << std::endl;
	output << "end_header" << std::endl;

	for (int p = 0; p < plen; p++)
	{
		std::string coordinates =
			std::to_string(particles[p].Xp[0]) + " " +
			std::to_string(particles[p].Xp[1]) + " " +
			"0";
		output << coordinates << std::endl;
	}

	output.close();
	std::cout << " Frame #: " << frame << std::endl;
}
