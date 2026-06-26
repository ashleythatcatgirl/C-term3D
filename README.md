# Cterm-3D

#### A very lightweight terminal program for rendering, editing, and exporting 3D models and scenes

Basically blender, but way worse
It's a project i decided to make cause i got really into the whole process of rendering things to the screen in fancy ways
It is still very much NOT a finished, or even presentable, program

## Compilation through gcc

### I can't promise this will actually compile on all systems, I use Arch linux so if ur not.. I wish u the best of luck.
```
gcc -lglfw -lm lassimp -o main.out main.c
```
I'm pretty sure the libraries used are either included or standard c libraries, if not.. i'll fix it eventually.
I'll release it as a compiled program at some point so it's actually presentable and not just souce code.

## Libraries and packages used
Rendering - [OpenGL 4.5 core](https://www.opengl.org/)

Window API - [GLFW](https://www.glfw.org/)

Image loading - [stb_image](https://github.com/nothings/stb)

Matrix and vector calculations - [CGLM](https://github.com/recp/cglm)

Loader generator - [GLAD](https://glad.dav1d.de/)

3D model file loader - [Assimp](https://github.com/assimp/assimp)

## Current features

### Rendering
- Basic 3D viewport scene
- Point lights
- Diffuse, specular, and normal maps

### Modeling
- none really ;w;

### Other
- Basic controls
- Basic commands (moving models around the scene)
- Importing models from files

## Future features

### Rendering
- Better lighting
- Shadows
- Post processing effects
- Better materials

### Modeling
- Editing individual verticies

### Other
- Exporting models
- Exportable scenes
- Ability to render scenes to img files
