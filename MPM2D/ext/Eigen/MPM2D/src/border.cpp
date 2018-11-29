#include "border.h"

/* Constructors */
Border::Border(const int inType, const Vector2f& inNormal, const std::vector<Vector2f>& inX_corner)
{
	type = inType;
	normal = inNormal;
	X_corner = inX_corner;
}



/* -----------------------------------------------------------------------
|						 COLLISIONS / FRICTIONS							 |
----------------------------------------------------------------------- */


void Border::Collision(const Vector2f& node_coordinates, Vector2f& node_velocity,
	std::vector<int> &collision, const int b)
{
	/* Current distance between node and boundary. */
	float distance = normal.dot(node_coordinates - X_corner[0]);
	/* If the node is inside the boundary, and the boundary is sticky,
	veclocity is 0 (motionless boundary). */
	if ((type == 1) && (distance < 0))
	{
		node_velocity = Vector2f(0, 0);
	}

	/* If not sticky, compute trial distance (after update of Xi). */
	else
	{
		Vector2f trial_position = node_coordinates + DT * node_velocity;
		float
			trial_distance = normal.dot(trial_position - X_corner[0]);
		float dist_c = trial_distance - std::min(distance, 0.0f);

		/* Record collision and update node velocity. */
		if (((type == 2) && (dist_c < 0)) || ((type == 3) && (distance < 0)))
		{
			node_velocity -= dist_c * normal / DT;
			collision.push_back(b);
		}
	}
}


/* Only for recorded collisions. */
void Border::Friction(Vector2f& Vi_fri, const Vector2f& Vi_col, const Vector2f& Vi)
{
	/* Compute tangential velocity. */
	Vector2f Vt = Vi_col - normal * (normal.dot(Vi_fri));
	if (Vt.norm() > 1e-7)
	{
		Vector2f t = Vt / Vt.norm();
		/* Apply Coulomb's friction to tangential velocity. */
		Vi_fri -= std::min(Vt.norm(), CFRI* (Vi_col - Vi).norm())*t;
	}
}



/* -----------------------------------------------------------------------
|							BOUNDARY DRAWING							 |
----------------------------------------------------------------------- */


void Border::DrawBorder()
{
	glLineWidth(4);
	glColor3f(.3f, 0.3f, 0.3f);

	glBegin(GL_LINES);
	glVertex2f(X_corner[0][0], X_corner[0][1]);
	glVertex2f(X_corner[1][0], X_corner[1][1]);
	glEnd();
}
