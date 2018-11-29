#pragma once

#include "constants.h"
#include <vector>
#include <GLFW\glfw3.h>
using namespace Eigen;

/* The border class defines the borders (lines in 2D).
Interaction nodes-particles / borders defined here (collision, friction...). */

class Border
{
public:

	/* Data */
	Vector2f normal;										// UNIT vector normal to the plan
	int type;												// [1] sticky - [2] Separating - [3] Sliding

	std::vector<Vector2f> X_corner;							// Vertices of the border



	/* Constructors */
	Border() {};
	Border(const int inType, const Vector2f& inNormal, const std::vector<Vector2f>& inX_corner);
	~Border() {};



	/* Functions */
	void Collision(const Vector2f& node_coordinates,		// Apply collision with border object
		Vector2f& node_velocity,
		std::vector<int>& collision,
		const int b);
	void Friction(Vector2f& Vi_fri,							// Apply friction with border object
		const Vector2f& Vi_col,
		const Vector2f& Vi);

	void DrawBorder();										// Draw border edges



	/* Static Functions */
	static std::vector<Border> InitializeBorders()			// Initialize array of borders
	{
		std::vector<Border> outBorders;
		std::vector<Vector2f> Corners;

		/* Left border */
		Corners.push_back(Vector2f(CUB, CUB));
		Corners.push_back(Vector2f(CUB, Y_GRID -  CUB));
		outBorders.push_back(Border(2, Vector2f(1, 0), Corners));
		Corners.clear();

		/* Right border */
		Corners.push_back(Vector2f(X_GRID - CUB, CUB));
		Corners.push_back(Vector2f(X_GRID - CUB, Y_GRID - CUB));
		outBorders.push_back(Border(2, Vector2f(-1, 0), Corners));
		Corners.clear();

		/* Bottom border */
		Corners.push_back(Vector2f(CUB, CUB));
		Corners.push_back(Vector2f(X_GRID - CUB, CUB));
		outBorders.push_back(Border(2, Vector2f(0, 1), Corners));
		Corners.clear();

		/* Top border */
		Corners.push_back(Vector2f(CUB, Y_GRID - CUB));
		Corners.push_back(Vector2f(X_GRID - CUB, Y_GRID - CUB));
		outBorders.push_back(Border(2, Vector2f(0, -1), Corners));
		Corners.clear();

		return outBorders;
	}
};
