
#ifndef MAIN_H
#define MAIN_H

#include "include.h"
#include "model.h"

const unsigned int INIT_WIDTH = 960;
const unsigned int INIT_HEIGHT = 640;

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

void UpdateLight(Light *light);

void CreateDebugPoint(Scene *scene);

void FreeMemory(Scene *scene);
void FreeModel(Model *model);
void FreeMesh(Mesh *mesh);

void *ResizeArray(void *array, unsigned int size);

void InitializeStructs(Window *window, Input *input, Camera* camera, Mouse *mouse, Controls *controls, Scene *scene);
int RenderLoop(Window *window, Input *input, Camera *camera, Scene *scene);

#endif
