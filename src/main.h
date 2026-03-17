
#ifndef MAIN_H
#define MAIN_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "include/stb_image.h"
#include "include/cglm/cglm.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <dirent.h>

#include <unistd.h>
#include <fcntl.h>

#include <regex.h>

const unsigned int INIT_WIDTH = 960;
const unsigned int INIT_HEIGHT = 640;

const char OBJECT_DIRECTORY[] = "../files/objects/";
const char TEXTURE_DIRECTORY[] = "../files/textures/";
const char SHADER_DIRECTORY[] = "../src/shaders/";

typedef struct Window {
	double delay;
	int width;
	int height;
	GLFWwindow *frame;
} Window;

typedef struct Input {
	char buffer[128];
	int length;
	unsigned int opts;
} Input;

typedef struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} Material;

typedef struct Model {
	unsigned int VBO;
	unsigned int VAO;

	unsigned int shader;

	unsigned int transformCount;
	vec3 *translate;
	vec3 *rotate;
	vec3 scale;

	vec3 color;
	unsigned int texture;

	Material material;
} Model;

typedef struct Models {
	unsigned int count;
	Model *model;
} Models;

typedef struct Texture {
	unsigned int memory;
	char name[64];
} Texture;

typedef struct Textures {
	unsigned int count;
	Texture *texture;
} Textures;

typedef struct Transforms {
	mat4 model;
	mat4 view;
	mat4 projection;
	
	int modelLoc;
	int viewLoc;
	int projectionLoc;
} Transforms;

typedef struct Camera {
	vec3 position;
	vec3 target;
	vec3 front;
	vec3 right;

	vec3 direction;

	float pitch;
	float yaw;

	float zoom;

	float moveSpeed;
	float turnSpeed;
} Camera;

typedef struct Mouse {
	float lastX;
	float lastY;

	float xOffset;
	float yOffset;

	float sensitivity;

	bool firstMouse;
} Mouse;

typedef struct Controls {
	Camera *camera;
	Mouse *mouse;
} Controls;

int ParseArgs(int argc, char **argv, struct Input *input);

int CreateWindow(Window *window, Controls *controls);

void SetModelData(Model *model);

char* GetShaderContent(const char* fileName);
int LoadShader(unsigned int *shaderProgram, const char *vertShader, const char *fragShader);

int LoadTextures(Textures *textures);
int LinkTextures(Textures *textures, unsigned int *shaderProgram);

int RenderLoop(Window *window, Input *input, Models *models, Textures *textures, Transforms *transforms, Camera *camera);
void processInput(Window *window, Camera *camera, float deltaTime);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

void FreeMemory(Models *models, Textures *textures);

void InitializeStructs(Window *window, Textures *textures, Models *models, Transforms *transforms, Camera* camera, Mouse *mouse, Controls *controls);

#endif
