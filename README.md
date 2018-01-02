# GPU-particle-system
The project is system of 2M textured particles. All physics is calculated on GPU with compute shader. Source code of shaders is in main.cpp.  
As for now only VS2017 project files are provided, but soon I will push some CMake for easy build for other platforms.  
## External projects used
* [GLFW](https://github.com/glfw/glfw) - OpenGL context, window creation and input handling
* [flextGL](https://github.com/ginkgo/flextGL) - OGL files generation
* some tga loading code by Michael Thomas Greer (tga.h, tga.c files)
* [fhl](https://github.com/Crisspl/fhl) - My 2.5D graphics library on which I've been learning basics of OpenGL (not finished yet, but usable). I took a few classes from there. All CPU maths (quaternion, matrix and vector calculations) are done with classes from fhl. Also delta-time measuring and OpenGL functions loading are done with fhl.  
  
See [YouTube video](https://youtu.be/TvJGFqh-pmM) with a litle 'gameplay'!
