# C-term3D

A very lightweight terminal program for rendering, editing, and exporting object files and models.

Basically blender, but waaaaaaay worse.
It's a project i decided to make cause i got really into the whole process of rendering things to the screen in fancy ways, also im a math nerd.

Also didn't update the repo in half a year lol, sowwyyyyyyyyyy.

## Compilation through gcc

# I can't promise this will actually compile on all systems, I use Arch linux so if ur not.. I wish u the best of luck.
```
gcc -lglfw -lm -o main.out main.c
```
I'm pretty sure the libraries used are either included or standard c libraries, if not.. i'll fix it eventually.
I'll release it as a compiled program at some point so it's actually presentable and not just souce code.

# Libraries and packages used
Rendering - [OpenGL 4.5 core](https://www.opengl.org/)

Window API - [GLFW](https://www.glfw.org/)

Image loading - [stb_image](https://github.com/nothings/stb)

Matrix and vector calculations - [CGLM](https://github.com/recp/cglm)

Loader generator - [GLAD](https://glad.dav1d.de/)

3D model (model file) loader - [Assimp](https://github.com/assimp/assimp)

## List of current features

### Rendering
- 3D viewport
- Loadable extures
- Semi-loadable shaders
- Multiple objects
#### Lighting
- Diffuse and specular maps
- Point lights
- Attenuation

### Modeling
- none really ;w;

### Other
- Transforms
- Camera controls (keyboard and mouse)
- Basic commands

## List of (Hopefully) future features

### Rendering
- Better lighting
- Shadows
- Post processing effects
- Better materials (normal maps, etc.)

### Modeling
- Selecting models
- Editing individual verticies

### Other
- Importing/exporting models
- Exportable scenes
- Ability to render scenes to img files
