#pragma once

#include "node.h"
#include "particle.h"
#include <iostream>
#include <fstream>
#include <iomanip>

/* The solver class is the link between particles and nodes.
Transfers and updates are executed on solver instances. */

class Solver
{
public:

	/* Data */
	std::vector<Border> borders;
	std::vector<Node> nodes;
	std::vector<Material> particles;

	size_t ilen, blen, plen;



	/* Constructors */
	Solver() {};
	Solver(const std::vector<Border>& inBorders, const std::vector<Node>& inNodes,
		const std::vector<Material>& inParticles);
	~Solver() {};



	/* Functions */
	void P2G();										// Transfer from Particles to Grid nodes
	void UpdateNodes();
	void G2P();										// Transfer from Grid nodes to Particles
	void UpdateParticles();
	void ResetGrid();

	void Draw();									// Draw particles, border and nodes (if selected)
	void WriteToFile(int frame);					// Write point cloud coordinates to .ply file (Houdini)



	/* Static functions */
	static float Bspline(float x)					// Cubic Bspline
	{
		float W;
		x = fabs(x);

		if (x < 1)
			W = (x*x*x / 2.0f - x * x + 2 / 3.0f);

		else if (x < 2)
			W = (2 - x)*(2 - x)*(2 - x) / 6.0f;

		else
			W = 0;

		return W;
	}


	static float dBspline(float x)					// Cubic Bspline derivative
	{
		float dW;
		float x_abs;
		x_abs = fabs(x);

		if (x_abs < 1)
			dW = 1.5f * x * x_abs - 2.0f * x;

		else if (x_abs < 2)
			dW = -x * x_abs / 2.0f + 2 * x - 2 * x / x_abs;

		else
			dW = 0;

		return dW;
	}


	static float getWip(const Vector2f& dist)		// 2D weight
	{
		return Bspline(dist[0]) * Bspline(dist[1]);
	}


	static Vector2f getdWip(const Vector2f& dist)	// 2D weight gradient
	{
		return Vector2f(
			dBspline(dist[0]) * Bspline(dist[1]),
			Bspline(dist[0]) * dBspline(dist[1]));
	}
};
