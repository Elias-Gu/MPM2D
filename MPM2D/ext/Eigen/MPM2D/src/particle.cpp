#include "particle.h"

/* Constructors */
Particle::Particle(const float inVp0, const float inMp,
	const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp)
{
	Vp0 = inVp0;
	Mp = inMp;

	Xp = inXp;
	Vp = inVp;
	Bp = inBp;
}


/* ---------------------------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
|								CONSTRUCTORS							 |
----------------------------------------------------------------------- */


Water::Water(const float inVp0, const float inMp,
	const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp)
	: Particle(inVp0, inMp, inXp, inVp, inBp)
{
	Ap = 0.0f;
	Jp = 1.0f;
}


Water::Water(Particle p)
	: Particle(p.Vp0, p.Mp, p.Xp, p.Vp, p.Bp)
{
	Ap = 0.0f;
	Jp = 1.0f;
}


//
DrySand::DrySand(const float inVp0, const float inMp,
	const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp)
	: Particle(inVp0, inMp, inXp, inVp, inBp)
{
	Ap = Matrix2f::Zero();

	Fe = Matrix2f::Identity(); FeTr = Matrix2f::Identity();
	Fp = Matrix2f::Identity(); FpTr = Matrix2f::Identity();

	q = 0.0f;
	float phi = H0 + (H1 * q - H3) * exp(-H2 * q);
	alpha = sqrt(2 / 3.0f) * 2 * sin(phi) / (3 - sin(phi));

	r = ((float)rand() / (RAND_MAX));
}


DrySand::DrySand(Particle p)
	: Particle(p.Vp0, p.Mp, p.Xp, p.Vp, p.Bp)
{
	Ap = Matrix2f::Zero();

	Fe = Matrix2f::Identity(); FeTr = Matrix2f::Identity();
	Fp = Matrix2f::Identity(); FpTr = Matrix2f::Identity();

	q = 0.0f;
	float phi = H0 + (H1 * q - H3) * exp(-H2 * q);
	alpha = sqrt(2 / 3.0f) * 2 * sin(phi) / (3 - sin(phi));

	r = ((float)rand() / (RAND_MAX));
}



/* -----------------------------------------------------------------------
|							PHYSICAL MODEL								 |
----------------------------------------------------------------------- */


// Water: http://www.math.ucla.edu/~jteran/papers/PGKFTJM17.pdf
void Water::ConstitutiveModel()
{
	float dJp = -K_water * (1.0f / pow(Jp, GAMMA_water) - 1.0f);	// Deformation gradient increment
	Ap = dJp * Vp0 * Jp;											// For computation and clarity
}


void Water::UpdateDeformation(const Matrix2f& T)
{
	Jp = (1 + DT * T.trace()) * Jp;
}


// Dry Sand: http://www.math.ucla.edu/~jteran/papers/KGPSJT16.pdf 
//TODO: faster with vector instead of matrix (super slow)
void DrySand::ConstitutiveModel()
{
	JacobiSVD<Matrix2f> svd(Fe, ComputeFullU | ComputeFullV);		// SVD decomposition
	Matrix2f Eps = svd.singularValues().asDiagonal();		
	// We have to pass through array() because log() not available on Matrices
	Matrix2f logEps = Eps.diagonal().array().log().matrix().asDiagonal();
	Matrix2f U = svd.matrixU();
	Matrix2f V = svd.matrixV();

	Matrix2f dFe = 
		2 * MU_dry_sand*Eps.inverse()*logEps + LAMBDA_dry_sand * logEps.trace()*Eps.inverse();

	Ap = Vp0 * U * dFe * V.transpose() * Fe.transpose();
}


void DrySand::UpdateDeformation(const Matrix2f& T)
{
	FeTr = (Matrix2f::Identity() + DT * T) * Fe;
	FpTr = Fp;

	DrySand::Plasticity();
}


void DrySand::Plasticity()
{
	JacobiSVD<Matrix2f> svd(FeTr, ComputeFullU | ComputeFullV);		// SVD decomposition
	Matrix2f Eps = svd.singularValues().asDiagonal();
	Matrix2f U = svd.matrixU();
	Matrix2f V = svd.matrixV();

	Matrix2f T; float dq;
	DrySand::Projection(Eps, &T, &dq);

	Fe = U * T * V.transpose();
	Fp = V * T.inverse() * Eps * V.transpose() * FpTr;

	q += dq;
	float phi = H0 + (H1 *q - H3)*exp(-H2 * q);
	alpha = (float)(sqrt(2.0 / 3.0) * (2.0 * sin(phi)) / (3.0f - sin(phi)));
}


void DrySand::Projection(const Matrix2f& Eps, Matrix2f* T, float* dq)
{
	Matrix2f e, e_c;

	e = Eps.diagonal().array().log().matrix().asDiagonal();
	e_c = e - e.trace() / 2.0f * Matrix2f::Identity();

	if (e_c.norm() < 1e-8 || e.trace() > 0) {
		T->setIdentity();
		*dq = e.norm();
		return;										// Projection to the tip of the cone
	}

	float dg = e_c.norm() 
		+ (LAMBDA_dry_sand + MU_dry_sand) / MU_dry_sand * e.sum() * alpha;

	if (dg <= 0) {
		*T = Eps;
		*dq = 0;
		return;										// No projection 
	}

	Matrix2f Hm = e - dg * e_c / e_c.norm();

	*T = Hm.diagonal().array().exp().matrix().asDiagonal();
	*dq = dg;
	return;											// Projection onto the yield surface
}



/* -----------------------------------------------------------------------
|								RENDERING								 |
----------------------------------------------------------------------- */


float Water::grey[3] = { 0.75f, 0.75f, 0.75f };						// Define color gradient
float Water::green[3] = { 0.2f, 0.8f, 0.8f };
float Water::blue[3] = { 0.3f, 0.7f, 1.0f };
float Water::h_color = 40.0f;
float Water::l_color = 20.0f;
float Water::d_color = h_color - l_color;


void Water::DrawParticle()
{
	glPointSize(12);

	float x = Vp.norm();

	if (x / l_color < 1)
		glColor3f(
		(green[0] - blue[0]) / l_color * x + blue[0],
		(green[1] - blue[1]) / l_color * x + blue[1],
		(green[2] - blue[2]) / l_color * x + blue[2]);
	else if (x / h_color < 1)
		glColor3f(
		(grey[0] - green[0]) / d_color * x + green[0] - l_color * (grey[0] - green[0]) / d_color,
		(grey[1] - green[1]) / d_color * x + green[1] - l_color * (grey[1] - green[1]) / d_color,
		(grey[2] - green[2]) / d_color * x + green[2] - l_color * (grey[2] - green[2]) / d_color);
	else
		glColor3f(grey[0], grey[1], grey[2]);

	glEnable(GL_POINT_SMOOTH);										// Round particles
	glBegin(GL_POINTS);
	glVertex2f(Xp[0], Xp[1]);
	glEnd();
}


//
void DrySand::DrawParticle()
{
	glPointSize(12);

	if (r <= 0.60f)
		glColor3f(0.88f, 0.66f, 0.37f);
	else if (r > 0.60f && r <= 0.8f)
		glColor3f(0.59f, 0.43f, 0.20f);
	else
		glColor3f(0.64f, 0.48f, 0.25f);

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glVertex2f(Xp[0], Xp[1]);
	glEnd();
}
