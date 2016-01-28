# 2-D Carrom in OpenGL/C++
2-D carrom game implemented using OPENGL-2 and C++

## Prerequisites
C+ OPENGL-2 Library

Core OpenGL Library - GL

OpenGL Utility Library (GLU)

OpenGL Utilities Toolkit (GLUT)

OpenGL Extension Wrangler Library (GLEW)

Each of the software package consists of:

    A header file: "gl.h" for core OpenGL, "glu.h" for GLU, and "glut.h" (or "freeglut.h") for GLUT, typically kept under "include\GL" directory.
    A static library: for example, in Win32, "libopengl32.a" for core OpenGL, "libglu32.a" for GLU, "libglut32.a" (or "libfreeglut.a" or "glut32.lib") for GLUT, typically kept under "lib" directory.
    An optional shared library: for example, "glut32.dll" (for "freeglut.dll") for GLUT under Win32, typically kept under "bin" or "c:\windows\system32".

It is important to locate the directory path and the actual filename of these header files and libraries in your operating platform in order to properly setup the OpenGL programming environment.

Setup can be found here in detailed:
https://www3.ntu.edu.sg/home/ehchua/programming/opengl/HowTo_OpenGL_C.html

## Instructions
### Install
Although an installed binary is available with this code but sill just to check if everything works type make on terminal to install the game.

### Play
Type ./carrom to play
### Controls and Rules
rules.txt has all the info about controls and rules of game scoring.                                                                                 
