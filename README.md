# MPM2D

2D implementation of the Material Point Method.
![water](https://github.com/Elias-Gu/MPM2D//raw/master/MPM2D/out/water.gif)
![snow](https://github.com/Elias-Gu/MPM2D//raw/master/MPM2D/out/snow.gif)
![elastic](https://github.com/Elias-Gu/MPM2D//raw/master/MPM2D/out/elastic.gif)
## Sections
- [Overview](#Overview)
- [Dependencies](#Dependencies)
- [Code structure](#Code-structure)
- [Implementation](#Implementation)
- [Options](#Options)
<br><br>

## Overview
C++ implementation of the Material Point Method.

The [Material Point Method](https://www.seas.upenn.edu/~cffjiang/research/mpmcourse/mpmcourse.pdf) is a numerical technique used to simulate the behavior of continuum materials.

The continuum body is described by a number a Lagrangian elements : the material points.
Kinematic equations are solved on the material points  
The material points are surrounded by a background Eulerian grid where the dynamic equations are solved.

It can be summarize in 4 main steps:
1. Transfer data from particles de grid nodes
2. Update node state (apply forces)
3. Transfer data from grid nodes to particles
4. Update particles state
<br><br>

## Dependencies
The following libraries are includes in the `ext/` directory:
- [OpenGL and GLFW](https://www.glfw.org/)
        - Visual.
- [Poisson Generator](https://github.com/corporateshark/poisson-disk-generator)
        - Initialize particle position.
- [Algebra](https://github.com/Elias-Gu/MPM2D/tree/master/MPM2D/ext/Algebra)
        - My own 2D linear algebra library.
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
        - Alternatively, `Eigen` can be used to replace `Algebra`, but it is not as fast. The source code is in `ext/Eigen/MPM2D/src/` (not updated).

The followings are optional dependencies :
- [ffmpeg](https://www.ffmpeg.org/)
        - Output .mp4 videos.
- [OpenMP](https://www.openmp.org/)
        - Parallel computing.
<br><br>

## Code structure
The code, located in `src/`, is structured as following:
- `main.cpp`: OpenGL context. Run simulation.
- `solver.h` and `solver.cpp`: MPM algorithm functions (transfers and updates). Rendering and WriteToFile.
- `node.h` and `node.cpp`: Class for grid nodes.
- `border.h` and `border.cpp`: Class for 2D linear borders. Collision and Friction.
- `particle.h` and `particle.cpp`: Class and subclasses for particles and materials. Constitutive model and deformation functions.
- `constants.h`: Option control and global constants.
<br><br>

## Implementation

#### Characteristics:
Here are the main features of this implementation:
- Sand, Water, Snow and purely elastic simulatiosn already implemented
- 2D.
- Affine-Particle-in-Cell ([APIC](http://www.math.ucla.edu/~jteran/papers/JSSTS15.pdf)) transfer type.
-  B-Spline Quadratic or Cubic interpolation functions (Quadratic is faster, but not as precise).
- Node forces are updated with an explicit method.
- The domain has to be a convex geometry (for collision detection).

#### Add material type:
It is easy to add a new type of material. In `particle.h` and `particle.cpp`, create a new subclasse of `Particle`. Beside constructors, the subclass must contain the following functions:
- In `particle.h`:
```C++
static std::vector<NewMaterial> InitializeParticles() {
        // Define initial particle mass, volume, position, velocity and acceleration
        std::vector<NewMaterial> outParticles;
        // ...
	return outParticles;
}
```
```C++
static std::vector<NewMaterial> AddParticles() {
        // Define mass, volume, position, velocity and acceleration of particles to add during the simulation
	std::vector<NewMaterial> outParticles;
        // ...
	return outParticles;
}
```


- In `particle.cpp`:
```C++
void NewMaterial::ConstitutiveModel() {
    // Update Ap (pre-update deformation gradient)
}
```

```C++
void NewMaterial::UpdateDeformation(const Matrix2f& T) {
    // Update deformation gradient. 
    // T is the sum of the close node velocity gradients.
    // Elasticity, Plasticity functions (return-mapping, hardening) ...
}
```
```C++
void NewMaterial::DrawParticle() {
    // OpenGL output of particle points
}
```

#### Change domain geometry:
The shape of the domain can be changed, but is has to follow this rules:
- It has to be [convex](https://www.easycalculation.com/maths-dictionary/images/convex-nonconvex-set.png).
- It has to be included in [`CUB` ; `X_GRID - CUB`] x [`CUB` ; `Y_GRID - CUB`], where `CUB` is the range of the interpolation function (2 for Cubic, 1.5 for Quadratic).
- Borders have to be straight lines.

To modify the domain, in `border.h`, use the `InitializeBorders` static function:
```C++
static std::vector<Border> InitializeBorders() {
        std::vector<Border> outBorders;
        std::vector<Vector2f> Corners;

        // New border line
	Corners.push_back(Vector2f(X1, Y1));    // First point
	Corners.push_back(Vector2f(X2, Y2));    // Second point
        // type can be [1](sticky), [2](Separating) or [3](Sliding)
        // normal has to be oriented inside the domain and normalized
	outBorders.push_back(Border(type, normal, Corners));
	Corners.clear();

        // Add other border

	return outBorders;
}
```
<br><br>

## Options
Here is a list of different options available. They can be modify in the `constants.h` file.
- Grid:
```C++
// Size of the domain
const static int X_GRID = 128;
const static int Y_GRID = 64;
```
- Particle:
```C++
// Select Particle subclass (material type). [Water], [DrySand], [Snow], [Elastic]
#define Material NewMaterial
```

- Transfer particles <-> grid:
```C++
// Interpolation type: [1] Cubic - [2] Quadratic
#define INTERPOLATION 1	
// Time-step (typically about 1e-4)
const static float DT = 0.0001f;
```
- Output (outputs will be generated in the `out/` directory):
```C++
// Generate a .mp4 of the OpenGL window
#define RECORD_VIDEO true
// Generate a .ply file with node coordinates
#define WRITE_TO_FILE false	
// Draw nodes (active nodes have a different color)
#define DRAW_NODES false        // not recommended (slow)
```
<br><br>

### In progress
I am working on a more complete version that will include:
- 3D 
- Sparse Grid parallelisme (CUDA - OpenACC or OpenMP)
- Implicit grid update
- Other transfer schemes (MLS-MPM, PolyPIC)