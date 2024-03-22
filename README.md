# guru
A simple framework for building 3D scenes in OpenGL.
<br>
<br>
## Libraries
Guru makes use of the following libraries:
- [GL](https://www.opengl.org/) (OpenGL, v4.6) - for rendering 2D and 3D vector graphics.
- [glad](https://github.com/Dav1dde/glad) (v2) - used for loading OpenGL function pointers, enabling cross-platform development.
- [GLFW](https://github.com/glfw/glfw) - provides low-level access for creating windows, creating contexts and surfaces, and reading input.
- [GLM](https://github.com/g-truc/glm) (OpenGL Mathematics, v0.9.9) - help with math for graphics that involve vectors and matrices.
- [SOIL](https://github.com/littlstar/soil) (Simple OpenGL Image Library) - loads and manipulates images for OpenGL.
- [ASSIMP](https://github.com/assimp/assimp) (Open Asset Import Library) - helps load 3D models into the program.
<br>

## Installing glad
1) Open the [generator for Glad.](https://gen.glad.sh/)
2) Set GL to `Version 4.6`.
3) Click `Add All` under the `Extensions` option.
4) Under `Options`, check the tickbox for `Loader`.
5) Download the resulting .zip file.
6) Extract the files, place the `include` and `src` folders under a directory `glad` in the external dependencies folder (e.g. `C:\Libraries`).
7) Copy `gl.c` from the `glad\src` directory and paste it into the project's main directory, then include it in the project.
<br>

## Project Settings with Visual Studio 2022 on Windows
### C/C++ -> General
Additional Include Directories:
- ...\glm
- ...\glfw\include

### Linker
General-> Additional Library Directories:
- ...\glfw\lib-vc2022

Input-> Additional Dependencies:
- opengl32.lib
- glfw3.lib

Command Line-> Additional Options:
- `/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /ignore:4099`

unneeded (?):
- user32.lib
- gdi32.lib
- shell32.lib
