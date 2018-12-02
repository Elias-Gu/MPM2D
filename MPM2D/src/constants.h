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
const static float DT = 0.0005f;						// Time-step

// Ouput
#define RECORD_VIDEO true
#define WRITE_TO_FILE false								// Write to file disables visual output
#define DRAW_NODES false								// Drawing node option

// Material
#define Material Snow									// [Water] - [DrySand]



/* -----------------------------------------------------------------------
|								CONSTANTS								 |
----------------------------------------------------------------------- */


/* ----- GRID ----- */
const static float H_INV = 1.0f;


/* ----- TRANSFER ----- */
#if INTERPOLATION == 1
const static int CUB = 2;
const static Vector2f Translation_xp = Vector2f(0.0f);
static const int bni = -1;
static const float Dp_scal = 3.0f;

#elif INTERPOLATION == 2
const static float CUB = 1.5f;
const static Vector2f Translation_xp = Vector2f(0.5f);
static const int bni = 0;
static const float Dp_scal = 4.0f;
#endif


/* ----- RENDERING ----- */
const static int X_WINDOW = 1400;						// Window size
const static int Y_WINDOW = X_WINDOW * Y_GRID / X_GRID;

#if RECORD_VIDEO || WRITE_TO_FILE
const static int FPS = 30;								// Video frame rate
const static float DT_render = 1.0f / FPS;
#else
const static float DT_render = DT * 30.0f;				// Rate of frame rendered (OpenGL)
#endif				


/* ----- MATERIAL POINTS ----- */
//TODO correct friction call
#if Material Water
#define FRICTION false
#else
#define FRICTION true
#endif

const static Vector2f G = Vector2f(0.0f, -9.81f);		// Gravity
const static float CFRI = 0.3f;							// Friction coefficient		


/* Water */
const static float RHO_water = 1.0f;					// Density
const static float K_water = 50.0f;						// Bulk Modulus
const static int   GAMMA_water = 3;						// Penalize deviation form incompressibility

const static int DT_ROB = 30;							// Rate of AddParticle()


/* Dry Sand */
static const float RHO_dry_sand = 1600.0f;				// Density
static const float E_dry_sand = 3.537e5;				// Young's modulus
static const float V_dry_sand = 0.3f;					// Poisson's ratio
static const float LAMBDA_dry_sand =					// Lame parameters
E_dry_sand * V_dry_sand / (1.0f + V_dry_sand) / (1.0f - 2.0f * V_dry_sand);
static const float MU_dry_sand = 
E_dry_sand / (1.0f + V_dry_sand) / 2.0f;

static const float PI = 3.1415927f;
static const float H0 = 35 * PI / 180.0f;				// Hardening parameters
static const float H1 = 9 * PI / 180.0f;
static const float H2 = 0.2f;
static const float H3 = 10 * PI / 180.0f;


/* Snow */
static const float THT_C_snow = 2.0e-2f;				// Critical compression
static const float THT_S_snow = 6.0e-3f;				// Critical stretch
static const float KSI_snow = 10;						// Hardening coefficient
static const float RHO_snow = 4.0e2;					// Density
static const float E_snow = 1.4e5;						// Young's modulus
static const float V_snow = 0.2f;						// Poisson's ratio
static const float LAM_snow =							// Lame parameters
E_snow * V_snow / (1.0f + V_snow) / (1.0f - 2.0f * V_snow); 
static const float MU_snow =
E_snow / (1.0f + V_snow) / 2.0f;
