#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "particle.h"
#include "node.h"

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
	#if INTERPOLATION == 1
	static double Bspline(double x)					// Cubic Bspline
	{
		double W;
		x = fabs(x);

		if (x < 1)
			W = (x*x*x / 2.0 - x * x + 2 / 3.0);

		else if (x < 2)
			W = (2 - x)*(2 - x)*(2 - x) / 6.0;

		else
			W = 0;

		return W;
	}


	static double dBspline(double x)					// Cubic Bspline derivative
	{
		double dW;
		double x_abs;
		x_abs = fabs(x);

		if (x_abs < 1)
			dW = 1.5 * x * x_abs - 2.0 * x;

		else if (x_abs < 2)
			dW = -x * x_abs / 2.0 + 2 * x - 2 * x / x_abs;

		else
			dW = 0;

		return dW;
	}


	#elif INTERPOLATION == 2
	static double Bspline(double x)
	{
		double W;
		x = fabs(x);

		if (x < 0.5)
			W = -x * x + 3 / 4.0;

		else if (x < 1.5)
			W = x * x / 2.0 - 3 * x / 2.0 + 9 / 8.0;
		
		else
			W = 0;

		return W;
	}


	static double dBspline(double x)
	{
		double dW;
		double x_abs;
		x_abs = fabs(x);

		if (x_abs < 0.5)
			dW = -2.0 * x;
		
		else if (x_abs < 1.5)
			dW = x - 3 / 2.0 * x / x_abs;
		
		else
			dW = 0;

		return dW;
	}
	#endif


	static double getWip(const Vector2f& dist)		// 2D weight
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
