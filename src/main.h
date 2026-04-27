
#ifndef MAIN_H
#define MAIN_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/stb_image.h"
#include "../include/cglm/cglm.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <fcntl.h>
#include <regex.h>

const unsigned int INIT_WIDTH = 960;
const unsigned int INIT_HEIGHT = 640;

const char OBJECT_DIRECTORY[] = "../files/objects/";
const char TEXTURE_DIRECTORY[] = "../files/textures/";
const char SHADER_DIRECTORY[] = "../src/shaders/";

typedef enum {
	OBJ_MODEL,
	OBJ_LIGHT_POINT,
	OBJ_LIGHT_DIRECTION
} ObjectType;

typedef struct Window {
	float delay;
	int width;
	int height;
	GLFWwindow *frame;
} Window;

typedef struct Regex {
	unsigned int count;
	regex_t *patterns;
} Regex;

typedef struct Input {
	char buffer[128];
	int length;
	unsigned int opts;
} Input;

typedef struct Material {
	unsigned int texture;

	float shininess;
} Material;
typedef struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;

	float attLinear;
	float attQuadratic;
} Light;

typedef struct Model2 {
	unsigned int VBO;
	unsigned int VAO;

	unsigned int shader;

	vec3 translate, rotate, scale;

	ObjectType type;
	union {
		Material material;
		Light light;
	} data;
} Model2;

typedef struct Models {
	unsigned int count;
	Model2 *model;
} Models;

typedef struct Tex {
	unsigned int memory;
	char name[64];
} Tex;
typedef struct Texture2 {
	Tex *diffuse;
	Tex *specular;

	unsigned int diffuseCount;
	unsigned int specularCount;
} Texture2;
typedef struct Textures {
	unsigned int count;
	Texture2 *texture;
} Textures;

typedef struct Camera {
	vec3 position;
	vec3 target;
	vec3 front;
	vec3 right;

	vec3 direction;

	float pitch;
	float yaw;

	float zoom;
} Camera;

typedef struct Mouse {
	float lastX;
	float lastY;

	float sensitivity;

	bool firstMouse;
} Mouse;

typedef struct Controls {
	Camera *camera;
	Mouse *mouse;
} Controls;



void SetModelData(Model2 *model);

int LoadTextures(Textures *textures);
int GenerateTexture(Tex *tex);

void UpdateLight(Model2 *light);


void FreeMemory(Models *models, Textures *textures);
void *ResizeArray(void *array, unsigned int size);


#endif
