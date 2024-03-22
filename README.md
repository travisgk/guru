# guru
A simple framework for building 3D scenes in OpenGL.
<br>
<br>
## Libraries
Guru makes use of the following libraries:
- [GL](https://www.opengl.org/) (OpenGL) - for rendering 2D and 3D vector graphics.
- [GLEW](https://github.com/nigels-com/glew) (OpenGL Extension Wrangler Library) - helps the program use advanced graphics features.
- [GLM](https://github.com/g-truc/glm) (OpenGL Mathematics) - help with math for graphics that involve vectors and matrices.
- [glad](https://github.com/Dav1dde/glad) - used for loading OpenGL function pointers, enabling cross-platform development.
- [SOIL](https://github.com/littlstar/soil) (Simple OpenGL Image Library) - loads and manipulates images for OpenGL.
- [GLFW](https://github.com/glfw/glfw) - provides low-level access for creating windows, creating contexts and surfaces, and reading input.
- [ASSIMP](https://github.com/assimp/assimp) (Open Asset Import Library) - helps load 3D models into the program.
<br>
<br>

## Project Settings with Visual Studio 2022 on Windows
### C/C++ -> General
Additional Include Directories:
- ...\glew\include
- ...\glm
- ...\glfw\include

### Linker
General-> Additional Library Directories:
- ...\glew\lib\Release\x64
- ...\glfw\lib-vc2022

Input-> Additional Dependencies:
- opengl32.lib
- glew32s.lib

Command Line-> Additional Options:
- `/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT`

unneeded (?):
- user32.lib
- gdi32.lib
- shell32.lib
