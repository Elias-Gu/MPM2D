#include "node.h"

/* Constructors */
Node::Node(const Vector2f& inXi)
{
	Mi = 0.0;

	Xi = inXi;
	Vi.setZeros();
	Vi_col.setZeros();
	Vi_fri.setZeros();

	Fi.setZeros();
}

std::vector<Border> Node::borders = Border::InitializeBorders();
size_t Node::blen = Node::borders.size();



/* -----------------------------------------------------------------------
|				 COLLISIONS / FRICTIONS  /  RESET						 |
----------------------------------------------------------------------- */


void Node::NodeCollisions()
{
	Vi_col = Vi;
	for (int b = 0; b < Node::blen; b++)
		borders[b].Collision(Xi, Vi_col, CollisionObjects, b);
}


void Node::NodeFrictions()
{
	Vi_fri = Vi_col;
	for (size_t i = 0, ilen = CollisionObjects.size(); i < ilen; i++)
		borders[CollisionObjects[i]].Friction(Vi_fri, Vi_col, Vi);
}


void Node::ResetNode()
{
	Mi = 0.0;
	Vi.setZeros();
	Fi.setZeros();
	CollisionObjects.clear();
}



/* -----------------------------------------------------------------------
|								RENDERING		     					 |
----------------------------------------------------------------------- */


void Node::DrawNode()
{
	glPointSize(3.0f);

	if (Mi > 0)
	{
		glColor3f(0.5f, 0.5f, 0.5f);
	}
	else
	{
		glColor3f(0.3f, 0.3f, 0.3f);
	}

	glBegin(GL_POINTS);
	glVertex2f(Xi[0], Xi[1]);
	glEnd();
}