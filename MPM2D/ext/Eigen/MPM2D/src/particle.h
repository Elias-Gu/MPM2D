#pragma once

#include "constants.h"
#include <PoissonGenerator/PoissonGenerator.h>
#include <math.h>										
#include <vector>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <GLFW/glfw3.h>
using namespace Eigen;

/* The particle class contains data commun to all simulation.
The material subclasses contains particular data and methods. */

class Particle
{
public:

	/* Data */
	float Vp0;												// Initial volume (cste)
	float Mp;												// Particle mass (cste)

	Vector2f Xp;											// Particle position	
	Vector2f Vp;											// Particle Velocity
	Matrix2f Bp;											// ~ Particle velocity field



	/* Constructors */
	Particle() {};
	Particle(const float inVp0, const float inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	~Particle() {};
};


/* ---------------------------------------------------------------------------------------------- */



/* WATER */
class Water : public Particle
{
public:

	/* Data */
	float Ap;												// For computation purpose
	float Jp;												// Deformation gradient (det)

	static float grey[3];									// Drawing colors
	static float green[3];
	static float blue[3];
	static float h_color;
	static float l_color;
	static float d_color;



	/* Constructors */
	Water() : Particle() {};
	Water(const float inVp0, const float inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	Water(Particle p);
	~Water() {};



	/* Functions */
	void ConstitutiveModel();								// Deformation gradient increment
	void UpdateDeformation(const Matrix2f& T);				// Deformation gradient update

	void DrawParticle();



	/* Static Functions */
	static std::vector<Water> InitializeParticles()
	{
		std::vector<Water> outParticles;
		return outParticles;
	}

	static std::vector<Water> AddParticles()				// Add particle mid-simulation
	{
		std::vector<Water> outParticles;
		Vector2f v = Vector2f(30, 0);						// Initial velocity
		Matrix2f a = Matrix2f::Zero();

		for (int p = 0; p < 8; p++)
		{
			float r = ((float)rand() / (RAND_MAX));			// random number

			Vector2f pos = Vector2f(CUB, Y_GRID - 2 * CUB - 0.5*p - r);		// new positions
			outParticles.push_back(Particle(1.14f, 0.0005f, pos, v, a));	
		}

		return outParticles;
	}
};



/* DRY SAND */
class DrySand : public Particle
{
public:

	/* Data */
	Matrix2f Ap;												// For computation purpose
	
	Matrix2f Fe, FeTr;											// (Trial) Elastic deformation
	Matrix2f Fp, FpTr;											// (Trial) Plastic deformation

	float q, alpha;												// Hardening paremeters

	float r;													// Color



	/* Constructors */
	DrySand() : Particle() {};
	DrySand(const float inVp0, const float inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	DrySand(Particle p);
	~DrySand() {};



	/* Functions */
	void ConstitutiveModel();								// Deformation gradient increment
	void UpdateDeformation(const Matrix2f& T);				// Deformation gradient update
	void Plasticity();										// Update plastic dissipation
	void Projection											// Return mapping algorithm
	(const Matrix2f& Eps, Matrix2f* T, float* dq);

	void DrawParticle();



	/* Static Functions */
	static std::vector<DrySand> InitializeParticles()
	{
		std::vector<DrySand> outParticles;
		DefaultPRNG PRNG;

		std::vector<sPoint> P_c = GeneratePoissonPoints(2000, PRNG);
		int NP = static_cast <int>(P_c.size());

		float W_COL = X_GRID / 8.0f;
		float H_COL = (Y_GRID - 2 * CUB) * 0.9f;
		float X_COL = (X_GRID - W_COL) / 2.0f;
		float Y_COL = CUB;

		float VOL = W_COL * H_COL / static_cast<float>(NP);
		float MASS = VOL * RHO_dry_sand / 100.0f;

		Vector2f v = Vector2f::Zero();							// Initial velocity
		Matrix2f a = Matrix2f::Zero();

		for (int p = 0; p < NP; p++)
		{
			Vector2f pos = Vector2f(P_c[p].x * W_COL + X_COL, P_c[p].y * H_COL + Y_COL);
			outParticles.push_back(DrySand(VOL, MASS, pos, v, a));
		}

		return outParticles;
	}


	static std::vector<DrySand> AddParticles()					// Add particle mid-simulation
	{
		std::vector<DrySand> outParticles;
		return outParticles;
	}				
};