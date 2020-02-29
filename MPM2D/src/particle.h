#pragma once

#include <math.h>										
#include <vector>

#include <GLFW/glfw3.h>
#include <PoissonGenerator/PoissonGenerator.h>

#include "constants.h"

/* The particle class contains data commun to all simulation.
The material subclasses contains particular data and methods. */

class Particle
{
public:

	/* Data */
	double Vp0;												// Initial volume (cste)
	double Mp;												// Particle mass (cste)

	Vector2f Xp;											// Particle position	
	Vector2f Vp;											// Particle Velocity
	Matrix2f Bp;											// ~ Particle velocity field



	/* Constructors */
	Particle() {};
	Particle(const double inVp0, const double inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	~Particle() {};
};


/* ---------------------------------------------------------------------------------------------- */



/* WATER */
class Water : public Particle
{
public:

	/* Data */
	double Ap;												// For computation purpose
	double Jp;												// Deformation gradient (det)

	static double grey[3];									// Drawing colors
	static double green[3];
	static double blue[3];
	static double h_color;
	static double l_color;
	static double d_color;



	/* Constructors */
	Water() : Particle() {};
	Water(const double inVp0, const double inMp,
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
			double r = ((double)rand() / (RAND_MAX));			// random number

			Vector2f pos = Vector2f((double)CUB, (double)(Y_GRID - 2 * CUB - 0.5*p - r));		// new positions
			outParticles.push_back(Particle(1.14, 0.0005, pos, v, a));	
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

	double q, alpha;												// Hardening paremeters

	double r;													// Color



	/* Constructors */
	DrySand() : Particle() {};
	DrySand(const double inVp0, const double inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp);
	DrySand(Particle p);
	~DrySand() {};



	/* Functions */
	void ConstitutiveModel();								// Deformation gradient increment
	void UpdateDeformation(const Matrix2f& T);				// Deformation gradient update
	void Plasticity();										// Update plastic dissipation
	void Projection											// Return mapping algorithm
	(const Vector2f& Eps, Vector2f* T, double* dq);

	void DrawParticle();



	/* Static Functions */
	static std::vector<DrySand> InitializeParticles()
	{
		std::vector<DrySand> outParticles;
		DefaultPRNG PRNG;

		std::vector<sPoint> P_c = GeneratePoissonPoints(2000, PRNG);
		int NP = static_cast <int>(P_c.size());

		double W_COL = X_GRID / 8.0;
		double H_COL = (Y_GRID - 2 * CUB) * 0.9;
		double X_COL = (X_GRID - W_COL) / 2.0;
		double Y_COL = CUB;

		double VOL = W_COL * H_COL / static_cast<double>(NP);
		double MASS = VOL * RHO_dry_sand / 100.0;

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
	double Je, Jp;												// Deformation gradients		

	double lam;													// Lame parameters
	double mu;

	double s, r;													// size and color



	/* Constructors */
	Snow() : Particle() {};
	Snow(const double inVp0, const double inMp,
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

		double R_BALL = fmin(double(X_GRID), double(Y_GRID)) * 0.33;
		double X_BALL = X_GRID * 0.3;
		double Y_BALL = Y_GRID * 0.45;

		double VOL = 2 * PI*R_BALL*R_BALL / static_cast<double>(NP);
		double MASS = VOL * RHO_snow / 100.0;

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
	double lam, mu;												// Lame parameter
	double r, g, b;												// color



	/* Constructors */
	Elastic() : Particle() {};
	Elastic(const double inVp0, const double inMp,
		const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp, 
		const double inlam, const double inmu, const double inr, const double ing, const double inb);
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

		std::vector<Vector2f> positions;					// Create cube point cloud
		for (double i = 0; i < fmax(X_GRID, Y_GRID) / 8.0; i ++)
			for (double j = 0; j < fmax(X_GRID, Y_GRID) / 8.0; j ++)
				positions.push_back(Vector2f(i, j));

		double VOL = (double)fmax(X_GRID, Y_GRID) * (double)fmax(X_GRID, Y_GRID) / 16.0;
		double MASS = VOL * RHO_elastic / 100.0;

		Vector2f v = Vector2f(30, 0);							// Initial velocity
		Matrix2f a = Matrix2f(0);

		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{														// 1st cube
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.1, positions[p][1] + Y_GRID / 3.0);
			outParticles.push_back(Elastic(VOL, MASS, pos, v, a, LAM_elastic*0.1, MU_elastic*0.1, 1, 0, 0));
		}
		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{														// 2nd cube
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.325, positions[p][1] + Y_GRID / 2.0);
			outParticles.push_back(Elastic(VOL, MASS, pos, v, a, LAM_elastic, MU_elastic, 0, 0, 1));
		}
		for (size_t p = 0, plen = positions.size(); p < plen; p++)
		{														// 3rd cube
			Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.55, positions[p][1] + Y_GRID * 2 / 3.0);
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
