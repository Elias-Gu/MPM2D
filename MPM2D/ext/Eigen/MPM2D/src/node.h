#pragma once

#include "constants.h"
#include "border.h"
#include <iostream>
using namespace Eigen;

/* The node class defines the background grid. */

class Node
{
public:

	/* Data */
	float Mi;											// Node mass

	Vector2f Xi;										// Node position
	Vector2f Vi;										// Node velocity, before update and after force
	Vector2f Vi_col;									// Node velocity, after collision
	Vector2f Vi_fri;									// Node velocity, after friction

	Vector2f Fi;										// Force applied to the node

	std::vector<int> CollisionObjects;					// List of borders on which the node collides



	/* Constructors */
	Node() {};
	Node(const Vector2f& inXi);
	~Node() {};



	/* Functions */
	void NodeCollisions();								// Apply collision to all borders
	void NodeFrictions();								// Apply friction if collision

	void ResetNode();
	void DrawNode();



	/* Static Functions */
	static std::vector<Border> borders;					// All the borders of the domain
	static size_t blen;

	static std::vector<Node> InitializeNodes()			// Initialize Nodes
	{
		std::vector<Node> outNodes;

		for (int y = 0; y <= Y_GRID; y++)
			for (int x = 0; x <= X_GRID; x++)
				outNodes.push_back(Node(Vector2f((float)x, (float)y)));

		return outNodes;
	}
};