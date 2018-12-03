#pragma once

#include "constants.h"
#include <PoissonGenerator/PoissonGenerator.h>
#include <math.h>										
#include <vector>
#include <GLFW/glfw3.h>

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
		Matrix2f a = Matrix2f(0);

		for (int p = 0; p < 8; p++)
		{
			float r = ((float)rand() / (RAND_MAX));			// random number

			Vector2f pos = Vector2f((float)CUB, (float)(Y_GRID - 2 * CUB - 0.5*p - r));		// new positions
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
	(const Vector2f& Eps, Vector2f* T, float* dq);

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

		Vector2f v = Vector2f(0);							// Initial velocity
		Matrix2f a = Matrix2f(0);

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



/* SNOW */
class Snow : public Particle
{
public:

	/* Data */
	Matrix2f Ap;												// For computation purpose

	Matrix2f Fe, FeTr;											// (Trial) Elastic deformation
	Matrix2f Fp, FpTr;											// (Trial) Plastic deformation
	float Je, Jp;												// Deformation gradients		

	float lam;													// Lame parameters
	float mu;

	float s, r;													// size and color



	/* Constructors */
	Snow() : Particle() {};
	Snow(const float inVp0, const float inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	Snow(Particle p);
	~Snow() {};



	/* Functions */
	void ConstitutiveModel();								// Deformation gradient increment
	void UpdateDeformation(const Matrix2f& T);				// Deformation gradient update
	void Plasticity();										// Update plastic dissipation

	void DrawParticle();



	/* Static Functions */
	static std::vector<Snow> InitializeParticles()
	{
		std::vector<Snow> outParticles;
		DefaultPRNG PRNG;

		std::vector<sPoint> P_c = GeneratePoissonPoints(3000, PRNG, 30, true);
		int NP = static_cast <int>(P_c.size());

		float R_BALL = fmin(float(X_GRID), float(Y_GRID)) * 0.33f;
		float X_BALL = X_GRID * 0.3f;
		float Y_BALL = Y_GRID * 0.45f;

		float VOL = 2 * PI*R_BALL*R_BALL / static_cast<float>(NP);
		float MASS = VOL * RHO_snow / 100.0f;

		Vector2f v = Vector2f(40, 0);							// Initial velocity
		Matrix2f a = Matrix2f(0);

		for (int p = 0; p < NP; p++)
		{
			Vector2f pos = Vector2f(P_c[p].x * R_BALL + X_BALL, P_c[p].y * R_BALL + Y_GRID - Y_BALL);
			outParticles.push_back(Snow(VOL, MASS, pos, v, a));
		}
		for (int p = 0; p < NP; p++)
		{
			Vector2f pos = Vector2f(P_c[p].x * R_BALL + X_GRID - X_BALL, P_c[p].y * R_BALL + Y_BALL);
			outParticles.push_back(Snow(VOL, MASS, pos, -v, a));
		}

		return outParticles;
	}


	static std::vector<Snow> AddParticles()					// Add particle mid-simulation
	{
		std::vector<Snow> outParticles;
		return outParticles;
	}
};



/* ELASTIC */
class Elastic : public Particle
{
public:

	/* Data */
	Matrix2f Ap;												// For computation purpose
	Matrix2f Fe;												// Elastic deformation
	float lam, mu;												// Lame parameter
	float r, g, b;												// color



	/* Constructors */
	Elastic() : Particle() {};
	Elastic(const float inVp0, const float inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp, 
		const float inlam, const float inmu, const float inr, const float ing, const float inb);
	Elastic(Particle p);
	~Elastic() {};



	/* Functions */
	void ConstitutiveModel();								// Deformation gradient increment
	void UpdateDeformation(const Matrix2f& T);				// Deformation gradient update

	void DrawParticle();



	/* Static Functions */
	static std::vector<Elastic> InitializeParticles()
	{
		std::vector<Elastic> outParticles;

		std::vector<Vector2f> positions;
		for (float i = 0; i < fmax(X_GRID, Y_GRID) / 8.0f; i ++)
			for (float j = 0; j < fmax(X_GRID, Y_GRID) / 8.0f; j ++)
				positions.push_back(Vector2f(i, j));

		float VOL = (float)fmax(X_GRID, Y_GRID) * (float)fmax(X_GRID, Y_GRID) / 16.0f;
		float MASS = VOL * RHO_elastic / 100.0f;

		Vector2f v = Vector2f(30, 0);							// Initial velocity
		Matrix2f a = Matrix2f(0);

		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.1f, positions[p][1] + Y_GRID / 3.0f);
			outParticles.push_back(Elastic(VOL, MASS, pos, v, a, LAM_elastic*0.1f, MU_elastic*0.1f, 1, 0, 0));
		}
		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.325f, positions[p][1] + Y_GRID / 2.0f);
			outParticles.push_back(Elastic(VOL, MASS, pos, v, a, LAM_elastic, MU_elastic, 0, 0, 1));
		}
		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.55f, positions[p][1] + Y_GRID * 2 / 3.0f);
			outParticles.push_back(Elastic(VOL, MASS, pos, v, a, 100*LAM_elastic, 100*MU_elastic, 0, 1, 0));
		}

		return outParticles;
	}


	static std::vector<Elastic> AddParticles()					// Add particle mid-simulation
	{
		std::vector<Elastic> outParticles;
		return outParticles;
	}
};
