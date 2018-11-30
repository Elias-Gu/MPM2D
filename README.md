# MPM2D

2D implementation of the Material Point Method.

![water](https://github.com/Elias-Gu/MPM2D//raw/master/MPM2D/out/water.gif)

---

## Overview
C++ implementation of the Material Point Method.
<!--
describe algorithm
-->
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

The following are optional dependencies :
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
- 2D.
- Affine-Particle-in-Cell ([APIC](http://www.math.ucla.edu/~jteran/papers/JSSTS15.pdf)) transfer type.
-  B-Spline Quadratic or Cubic interpolation functions (Quadratic is faster, but not as precise).
- Node forces are updated with an explicit method.
- The domain has to be a convex geometry (for collision detection).

#### Add material type:
It is easy to add a new type of material. In `particle.h` and `particle.cpp`, create a new subclasse of `Particle`. Beside constructors, the subclass must contain the following functions:
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

<!--
## Implementation(how to add particles type) 
### Papers
### options (output dir)

## References (autres implementation)

## In the work (3D, open ACC, implicit, autre transfer sheme)
-->




<!--
- Papier de base
- Matierials and papier
-
- Autres implementations interessantes
Screenshots
options (record, output, materials, Eigen)
structure pour update
dependencies (*ffmpeg, *eigen, *opengl, *openMp)
-->