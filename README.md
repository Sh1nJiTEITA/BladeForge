# Blade Profiling Methodology for Turbomachinery

This repository contains the implementation and research materials developed as part of a master's thesis project on **two- and three-dimensional blade profiling for turbomachinery**.  
The work was carried out at the **National Research University "Moscow Power Engineering Institute (MPEI)"** in 2025.

https://github.com/user-attachments/assets/fac981bf-1a47-44c5-b420-def3335841ee

## Overview

Turbomachinery blades are critical structural elements that directly affect the efficiency, reliability, and lifetime of turbines and compressors. Accurate geometric modeling of blades is therefore essential for the design of modern energy and propulsion systems.

This project focuses on the development of a **new profiling methodology** that combines:

- **Vector and linear algebra** techniques for geometric transformations,  
- **Parametric Bezier curves** for smooth contour generation,  
- **Cross-section based modeling** of blade geometry,  
- **3D lofting techniques** to construct complete blade surfaces.

In addition, the study explores the potential of **neural networks** to automate the profiling process by learning from datasets of existing blade sections and atlas profiles.

## Key Features

- **2D blade section generation** based on parametric geometric rules.  
- **3D blade modeling methodology** with support for twisted blades.  
- **Bezier curve operations** (degree elevation/reduction, resampling, discretization).  
- **Mathematical tools** for rotations, translations, and vector-based geometry.  
- **Automation experiments** using machine learning for blade reconstruction and modification.  
- **Verification** performed on the last-stage blades of a steam turbine and on modified atlas profiles.  

## Structure

The methodology is divided into several parts:
1. **Theoretical foundations** — literature review, blade profiling theory, and parametric geometry.  
2. **Mathematical methods** — vector algebra, matrices, Bezier curves, and discretization techniques.  
3. **2D section profiling** — construction of chord, leading/trailing edges, and curvature profiles.  
4. **3D blade generation** — section alignment and loft-based 3D modeling.  
5. **Neural network automation** — training and testing on custom datasets.  
6. **Validation** — reconstruction and modification of real turbine blade profiles.  

## Installation
### Linux & Unix-like systems
Was written and tested inside this environment.
Due to great number of dependencies such as OpenBLAS, Lapack and even OpenCASCADE installation process can be frustrating and unfunny. All dependencies must be installed inside system path or by package manager.
List of all needed dependencies can be found inside `./external/CMakeLists.txt`.

Basic installation steps is:
```bash
git clone https://github.com/Sh1nJiTEITA/BladeForge.git
cd BladeForge
cmake -S . -B build
cmake --build build
```
Command `cmake -S . -B build` with high chance will emit a number of linking/finding dependencies issues which might be solved to build application.

Inside this repo there are vcpkg files (old logic) which in some moment worked at both `linux` and `windows` system. If building from source (the only way) with already installed packages cant be done - using vcpkg logic can help.
### Windows
Building application on Window can be done with a lot of tweeks with dependencies or vcpkg.


# TODO List
- Add vertices input/output. Vertex output should be used for next steps in analysis. Input vertices should be used as stencil for designing new blades;
- Add screenshoting for ready blades;
