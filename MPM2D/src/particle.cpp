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
	Ap.setZeros();

	Fe.setIdentity(); FeTr.setIdentity();
	Fp.setIdentity(); FpTr.setIdentity();

	q = 0.0f;
	float phi = H0 + (H1 * q - H3) * exp(-H2 * q);
	alpha = sqrt(2 / 3.0f) * 2 * sin(phi) / (3 - sin(phi));

	r = ((float)rand() / (RAND_MAX));
}


DrySand::DrySand(Particle p)
	: Particle(p.Vp0, p.Mp, p.Xp, p.Vp, p.Bp)
{
	Ap.setZeros();

	Fe.setIdentity(); FeTr.setIdentity();
	Fp.setIdentity(); FpTr.setIdentity();

	q = 0.0f;
	float phi = H0 + (H1 * q - H3) * exp(-H2 * q);
	alpha = sqrt(2 / 3.0f) * 2 * sin(phi) / (3 - sin(phi));

	r = ((float)rand() / (RAND_MAX));
}


//
Snow::Snow(const float inVp0, const float inMp,
	const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp)
	: Particle(inVp0, inMp, inXp, inVp, inBp)
{
	Ap.setZeros();

	Fe.setIdentity(); FeTr.setIdentity();
	Fp.setIdentity(); FpTr.setIdentity();
	Je = 1.0f; Jp = 1.0f;

	lam = LAM_snow;
	mu = MU_snow;

	s = ((float)rand() / (RAND_MAX)) * 7;
	r = 1 - ((float)rand() / (RAND_MAX)) * 0.23f;
}


Snow::Snow(Particle p)
	: Particle(p.Vp0, p.Mp, p.Xp, p.Vp, p.Bp)
{
	Ap.setZeros();

	Fe.setIdentity(); FeTr.setIdentity();
	Fp.setIdentity(); FpTr.setIdentity();
	Je = 1.0f; Jp = 1.0f;

	lam = LAM_snow;
	mu = MU_snow;

	s = ((float)rand() / (RAND_MAX)) * 7;
	r = 1 - ((float)rand() / (RAND_MAX)) * 0.23f;
}


//
Elastic::Elastic(const float inVp0, const float inMp,
	const Vector2f& inXp, const Vector2f& inVp, const Matrix2f& inBp,
	const float inlam, const float inmu, const float inr, const float ing, const float inb)
	: Particle(inVp0, inMp, inXp, inVp, inBp)
{
	Ap.setZeros();
	Fe.setIdentity(); 
	lam = inlam; mu = inmu;
	r = inr; g = ing; b = inb;
}


Elastic::Elastic(Particle p)
	: Particle(p.Vp0, p.Mp, p.Xp, p.Vp, p.Bp)
{
	Ap.setZeros();
	Fe.setIdentity();
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
void DrySand::ConstitutiveModel()
{
	Matrix2f U, V;
	Vector2f Eps;
	Fe.svd(&U, &Eps, &V);								// SVD decomposition

	Vector2f dFe = 2 * MU_dry_sand * Eps.inv()*Eps.log() + LAMBDA_dry_sand * Eps.log().sum() * Eps.inv();

	Ap = Vp0 * U.diag_product(dFe) * V.transpose() * Fe.transpose();
}


void DrySand::UpdateDeformation(const Matrix2f& T)
{
	FeTr = (Matrix2f(1, 0, 0, 1) + DT * T) * Fe;
	FpTr = Fp;

	DrySand::Plasticity();
}


void DrySand::Plasticity()
{
	Matrix2f U, V;
	Vector2f Eps;
	FeTr.svd(&U, &Eps, &V);

	Vector2f T; float dq;
	DrySand::Projection(Eps, &T, &dq);

	// Elastic and plastic state
	Fe = U.diag_product(T) * V.transpose();
	Fp = V.diag_product_inv(T).diag_product(Eps) * V.transpose() * FpTr;

	// hardening
	q += dq;
	float phi = H0 + (H1 *q - H3)*exp(-H2 * q);
	alpha = (float)(sqrt(2.0 / 3.0) * (2.0 * sin(phi)) / (3.0f - sin(phi)));
}


void DrySand::Projection(const Vector2f& Eps, Vector2f* T, float* dq)
{
	Vector2f e, e_c;

	e = Eps.log();
	e_c = e - e.sum() / 2.0f * Vector2f(1);

	if (e_c.norm() < 1e-8 || e.sum() > 0) {
		T->setOnes();
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

	Vector2f Hm = e - dg * e_c / e_c.norm();

	*T = Hm.exp();
	*dq = dg;
	return;											// Projection onto the yield surface
}


// Snow: https://www.math.ucla.edu/~jteran/papers/SSCTS13.pdf
// http://alexey.stomakhin.com/research/siggraph2013_tech_report.pdf
void Snow::ConstitutiveModel()
{
	Matrix2f Re, Se;
	Fe.polar_decomp(&Re, &Se);

	Matrix2f dFe = 2 * mu*(Fe - Re)* Fe.transpose() + lam * (Je - 1) * Je * Matrix2f(1, 0, 0, 1);
	Ap = dFe * Vp0;
}


void Snow::UpdateDeformation(const Matrix2f& T)
{
	FeTr = (Matrix2f(1, 0, 0, 1) + DT * T) * Fe;
	FpTr = Fp;

	Snow::Plasticity();
}


void Snow::Plasticity()
{
	Matrix2f U, V;
	Vector2f Eps;
	FeTr.svd(&U, &Eps, &V);
	
	Vector2f T = Eps.clamp(1 - THT_C_snow, 1 + THT_S_snow);		// Projection

	Fe = U.diag_product(T) * V.transpose();
	Fp = V.diag_product_inv(T).diag_product(Eps) * V.transpose() * FpTr;

	Je = Fe.det();		
	Jp = Fp.det();

	// Hardening
	float exp = std::exp(KSI_snow*(1.0f - Jp));
	lam = LAM_snow * exp;
	mu = MU_snow * exp;
}


// Elastic
void Elastic::ConstitutiveModel()
{
	Matrix2f Re, Se;
	Fe.polar_decomp(&Re, &Se);
	float Je = Fe.det();

	Matrix2f dFe = 2 * mu*(Fe - Re)* Fe.transpose() +  lam * (Je - 1) * Je * Matrix2f(1, 0, 0, 1);
	Ap = dFe * Vp0;
}

void Elastic::UpdateDeformation(const Matrix2f& T)
{
	Fe = (Matrix2f(1, 0, 0, 1) + DT * T) * Fe;
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


//
void Snow::DrawParticle()
{
	glPointSize(s);
	glColor3f(r, r, r);

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glVertex2f(Xp[0], Xp[1]);
	glEnd();
}


//
void Elastic::DrawParticle()
{
	glPointSize(10);
	glColor3f(r*0.8f, g*0.8f, b*0.8f);

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glVertex2f(Xp[0], Xp[1]);
	glEnd();
}
