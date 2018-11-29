#pragma once

#include <Eigen/Dense>
using namespace Eigen;

/* -----------------------------------------------------------------------
|								   GRID									 |
----------------------------------------------------------------------- */


const static int X_GRID = 128;							// Size of the domain
const static int Y_GRID = 32;

const static float H_INV = 1.0f;



/* -----------------------------------------------------------------------
|								TRANSFER								 |
----------------------------------------------------------------------- */


const static float DT = 0.001f;						// Time-step

const static int CUB = 2;
const static Vector2f Translation_xp = Vector2f::Zero();
static const int bni = -1;
static const float Dp_scal = 3.0f;



/* -----------------------------------------------------------------------
|							    RENDERING								 |
----------------------------------------------------------------------- */
#define RECORD_VIDEO false
#define WRITE_TO_FILE false								// Write to file disables visual output
#define DRAW_NODES false								// Drawing node option


const static int X_WINDOW = 1800;						// Window size
const static int Y_WINDOW = X_WINDOW * Y_GRID / X_GRID;

#if RECORD_VIDEO || WRITE_TO_FILE
const static int FPS = 30;								// Video frame rate
const static float DT_render = 1.0f / FPS;
#else
const static float DT_render = DT * 30.0f;				// Rate of frame rendered (OpenGL)
#endif				



/* -----------------------------------------------------------------------
|							  MATERIAL POINTS							 |
----------------------------------------------------------------------- */
#define Material Water
#define FRICTION false

const static Vector2f G = Vector2f(0.0f, -9.81f);		// Gravity
const static float CFRI = 0.3f;							// Friction coefficient		


/* Water */
const static float RHO_water = 1.0f;					// Density
const static float K_water = 50.0f;						// Bulk Modulus
const static int   GAMMA_water = 3;						// Penalize deviation form incompressibility

const static int DT_ROB = 30;


/* Dry Sand */
static const float RHO_dry_sand = 1600.0f;
static const float E_dry_sand = 3.537e5;
static const float V_dry_sand = 0.3f;
static const float LAMBDA_dry_sand = 
E_dry_sand * V_dry_sand / (1.0f + V_dry_sand) / (1.0f - 2.0f * V_dry_sand);
static const float MU_dry_sand = 
E_dry_sand / (1.0f + V_dry_sand) / 2.0f;

static const float PI = 3.1415927f;
static const float H0 = 35 * PI / 180.0f;
static const float H1 = 9 * PI / 180.0f;
static const float H2 = 0.2f;
static const float H3 = 10 * PI / 180.0f;
