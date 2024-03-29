# Fluid (WIP)

Various (work in progress) fluid simulations in 2D, implemented using C++ with OpenGL and GLFW compiled to WebAssembly.

The first of these is the [Lattice-Boltzmann fluid simulation](https://marl0ny.github.io/wasm-fluid2d/LatticeBoltzmann/index.html), where the primary reference for this is Daniel Schroeder's [instructional document](http://physics.weber.edu/schroeder/javacourse/LatticeBoltzmann.pdf) for his javascript+HTML5 [Fluid Dynamics Simulation](https://physics.weber.edu/schroeder/fluids/), on which this particular simulation is based on.

As well as the Lattice-Boltzmann fluid simulation, a [stable fluids implementation](https://marl0ny.github.io/wasm-fluid2d/StableFluids/index.html) is also included in this project. This is made by following [Chapter 38. Fast Fluid Dynamics Simulation on the GPU](https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch38.html) of Nvidia's book <i>GPU Gems</i>, where the chapter is written by Mark J. Harris. The original [stable fluid algorithm](https://doi.org/10.1145/311535.311548) was first conceived by Jos Stam. An additional inspiration is the [WebGL Fluid Simulation](https://paveldogreat.github.io/WebGL-Fluid-Simulation/) by [PavelDoGreat](https://github.com/PavelDoGreat).

The [opengl-canvas-wasm](https://github.com/timhutton/opengl-canvas-wasm) example by [timhutton](https://github.com/timhutton/opengl-canvas-wasm) was particularly helpful in learning how to get a WebAssembly program to run.

### TODO
 - [x] Stable fluids
 - [ ] Add barriers for stable fluids
 - [x] Lattice-Boltzmann
 - [ ] FVM 
 - [ ] Other Finite Difference Methods
 - [ ] FEM
 - [ ] SPH
 - [ ] Relativistic fluids
