# guru
A simple framework for building 3D scenes in OpenGL.
<br>
<br>
## Libraries
Guru makes use of the following libraries:
- [GL](https://www.opengl.org/) (OpenGL, v4.6) - for rendering 2D and 3D vector graphics.
- [glad](https://github.com/Dav1dde/glad) (v2) - used for loading OpenGL function pointers, enabling cross-platform development.
- [GLFW](https://github.com/glfw/glfw) (v3.3.9) - provides low-level access for creating windows, creating contexts and surfaces, and reading input.
- [GLM](https://github.com/g-truc/glm) (OpenGL Mathematics, v0.9.9) - help with math for graphics that involve vectors and matrices.
- [SOIL](https://github.com/littlstar/soil) (Simple OpenGL Image Library, v1.16) - loads and manipulates images for OpenGL.
- [ASSIMP](https://github.com/assimp/assimp) (Open Asset Import Library) - helps load 3D models into the program.
<br>

## Installing glad
1) Open the [generator for glad.](https://gen.glad.sh/)
2) Set GL to `Version 4.6`.
3) Set the option next to it to `Core`.
4) Under `Options`, check the tickbox for `Loader`.
5) Click `Generate` and then download the resulting .zip file.
6) Extract the files, place the `include` and `src` folders under a directory `glad` in the external dependencies folder (e.g. `C:\Libraries`).
7) Copy `gl.c` from the `glad\src` directory, paste it into the project's main directory, and then add it to the Visual Studio project.
<br>


## Installing ASSIMP
1) Download the [CMake gui](https://cmake.org/download) if you don't already have it.
2) Download the latest release of [ASSIMP](https://github.com/assimp/assimp) as a .zip file.
3) Unzip the download to some location.
4) Open up the CMake GUI and give the following:
   - Where is the source code: ...\assimp-master
   - Where to build the binaries: ...\assimp-master\build
5) Click `Configure`.
   - If it asks to create a new directory, click accept.
   - Choose the corresponding compiler.
   - If it gives errors, click `Configure` again.
   - Click `Generate`.
6) Go to the build directory (...\assimp-master\build). Open the `Assimp.sln` file in Visual Studio.
   - This will open several projects at once.
   - Make sure your configuration is set to x64 and your choice of Debug/Release.
   - Highlight the ALL_BUILD project and build the solution for it.
7) Create a new folder in the external dependencies folder named `assimp` (e.g. `C:\Libraries\assimp`).
8) Navigate to ...\assimp-master\build.
   - Copy the `include` directory into the new ...\assimp directory.
   - Copy the files inside `lib\Debug` or `lib\Release` and paste them into the new ...\assimp directory.
9) Navigate to ...\assimp-master\build\bin. Copy and paste the .dll file into the project's main directory.
10) Navigate to ...\assimp-master (not the build folder).
    - Copy the `include` directory into the new ...\assimp directory.

These instructions are modified from those given by [this user.](https://github.com/michaelg29/yt-tutorials/blob/master/CPP/OpenGL/install.md#open-asset-import-library-assimp)
<br>
<br>

## Project Settings with Visual Studio 2022 on Windows
### General
C++ Language Standard:
- `ISO C++ 20 Standard`

### C/C++ -> General
Additional Include Directories:
- ...\glad\include
- ...\glm
- ...\glfw\include
- ...\soil\include
- ...\assimp\include

### Linker
General-> Additional Library Directories:
- ...\glfw\lib-vc2022
- ...\assimp\lib

Input-> Additional Dependencies:
- opengl32.lib
- glfw3.lib
- assimp-vc143-mtd.lib

Command Line-> Additional Options:
- `/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /ignore:4099`

unneeded (?):
- user32.lib
- gdi32.lib
- shell32.lib
