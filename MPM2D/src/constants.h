#pragma once

#include <Algebra/algebra.h>


/* -----------------------------------------------------------------------
|									OPTIONS								 |
----------------------------------------------------------------------- */

// Grid
const static int X_GRID = 200;							// Size of the domain
const static int Y_GRID = 100;

// Transfer
#define INTERPOLATION 1									// [1] Cubic - [2] Quadratic
const static double DT = 0.001;						// Time-step

// Ouput
#define RECORD_VIDEO false
#define WRITE_TO_FILE false								// Write to file disables visual output
#define DRAW_NODES false								// Drawing node option

// Material
#define Material Water									// [Water] - [DrySand] - [Snow] - [Elastic]



/* -----------------------------------------------------------------------
|								CONSTANTS								 |
----------------------------------------------------------------------- */


/* ----- GRID ----- */
const static double H_INV = 1.0;


/* ----- TRANSFER ----- */
#if INTERPOLATION == 1
const static int CUB = 2;
const static Vector2f Translation_xp = Vector2f(0.0);
static const int bni = -1;
static const double Dp_scal = 3.0;

#elif INTERPOLATION == 2
const static double CUB = 1.5;
const static Vector2f Translation_xp = Vector2f(0.5);
static const int bni = 0;
static const double Dp_scal = 4.0;
#endif


/* ----- RENDERING ----- */
const static int X_WINDOW = 1400;						// Window size
const static int Y_WINDOW = X_WINDOW * Y_GRID / X_GRID;

#if RECORD_VIDEO || WRITE_TO_FILE
const static int FPS = 30;								// Video frame rate
const static double DT_render = 1.0 / FPS;
#else
const static double DT_render = DT * 30.0;				// Rate of frame rendered (OpenGL)
#endif				


/* ----- MATERIAL POINTS ----- */
//TODO correct friction call
#if Material Water
#define FRICTION false
#else
#define FRICTION true
#endif

const static Vector2f G = Vector2f(0.0f, -9.81);		// Gravity
const static double CFRI = 0.3;							// Friction coefficient		


/* Water */
const static double RHO_water = 1.0;					// Density
const static double K_water = 50.0;						// Bulk Modulus
const static int   GAMMA_water = 3;						// Penalize deviation form incompressibility

const static int DT_ROB = 30;							// Rate of AddParticle()


/* Dry Sand */
static const double RHO_dry_sand = 1600.0;				// Density
static const double E_dry_sand = 3.537e5;				// Young's modulus
static const double V_dry_sand = 0.3;					// Poisson's ratio
static const double LAMBDA_dry_sand =					// Lame parameters
E_dry_sand * V_dry_sand / (1.0 + V_dry_sand) / (1.0 - 2.0 * V_dry_sand);
static const double MU_dry_sand = 
E_dry_sand / (1.0 + V_dry_sand) / 2.0;

static const double PI = 3.14159265358979323846;
static const double H0 = 35 * PI / 180.0;				// Hardening parameters
static const double H1 = 9 * PI / 180.0;
static const double H2 = 0.2;
static const double H3 = 10 * PI / 180.0;


/* Snow */
static const double THT_C_snow = 2.0e-2;				// Critical compression
static const double THT_S_snow = 6.0e-3;				// Critical stretch
static const double KSI_snow = 10;						// Hardening coefficient
static const double RHO_snow = 4.0e2;					// Density
static const double E_snow = 1.4e5;						// Young's modulus
static const double V_snow = 0.2;						// Poisson's ratio
static const double LAM_snow =							// Lame parameters
E_snow * V_snow / (1.0 + V_snow) / (1.0 - 2.0 * V_snow); 
static const double MU_snow =
E_snow / (1.0 + V_snow) / 2.0;


/* Elastic */
static const double RHO_elastic = 4.0e2;					// Density
static const double E_elastic = 1.4e5;					// Young's modulus
static const double V_elastic = 0.2;					// Poisson's ratio
static const double LAM_elastic =						// Lame parameters
E_elastic * V_elastic / (1.0 + V_elastic) / (1.0 - 2.0 * V_elastic);
static const double MU_elastic =
E_elastic / (1.0 + V_elastic) / 2.0;
