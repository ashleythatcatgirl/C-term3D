
#include "main.h"

#include "../include/glad.c"

#include "include.h"
#include "parseInput.c"
#include "controls.c"
#include "shader.c"
#include "window.c"
#include "model.c"
#include <stdio.h>

int main() {
	struct Window window;
	struct Input input;

	struct Camera camera;
	struct Mouse mouse;
	struct Controls controls;

	struct Scene scene;
	
	InitializeStructs(&window, &input, &camera, &mouse, &controls, &scene);

	switch(CreateWindow(&window, &controls)) {
	case 0:
		printf("Window created successfully\n");
		break;
	case 1:
		printf("Window failed to create\n");
		goto exitProgram;
	}

	printf("loading model 0\n");
	stbi_set_flip_vertically_on_load(true);
	LoadModel(&scene.models[0], "../models/backpack/backpack.obj");
	glm_vec3_copy((vec3){5.0, 1.7, 0.0}, scene.models[0].position);

	printf("loading model 1\n");
	stbi_set_flip_vertically_on_load(false);
	LoadModel(&scene.models[1], "../models/tannhauser/tannhauser.obj");
	glm_vec3_copy((vec3){0.05, 0.05, 0.05}, scene.models[1].scale);
	glm_vec3_copy((vec3){0.0, 1.0, 0.0}, scene.models[1].rotation);

	printf("loading model 2\n");
	LoadModel(&scene.models[2], "../models/dragon/dragon.obj");
	glm_vec3_copy((vec3){-5.0, 1.7, 0.0}, scene.models[2].position);
	glm_vec3_copy((vec3){30.0, 30.0, 30.0}, scene.models[2].scale);
	glm_vec3_copy((vec3){0.0, 1.0, 0.0}, scene.models[2].rotation);
	glm_vec3_normalize(scene.models[2].rotation);
	scene.models[2].rotationAngle = M_PI / 2;

	printf("loading model 3\n");
	LoadModel(&scene.models[3], "../models/cube_1/cube1.obj");
	glm_vec3_copy((vec3){0.0, 10.0, 0.0}, scene.models[3].position);

	CreateDebugPoint(&scene);

	printf("Loading model shaders\n");
	for (unsigned int m = 0; m < scene.mCount; m++) {
		LoadShader(&scene.models[m].shader, (const char*[3]){"shaders/vertex/model.glsl", "", "shaders/fragment/model.glsl"});
	}
	printf("Loading light shaders\n");
	for (unsigned int l = 0; l < scene.lCount; l++) {
		LoadShader(&scene.lights[l].shader, (const char*[3]){"shaders/vertex/light.glsl", "shaders/geometry/light.glsl", "shaders/fragment/light.glsl"});
	}

	LoadShader(&scene.floorShader, (const char*[3]){"shaders/vertex/floor.glsl", "shaders/geometry/floor.glsl", "shaders/fragment/floor.glsl"});

	printf("Loading successful, press enter to continue..");
	getchar();

	RenderLoop(&window, &input, &camera, &scene);

exitProgram:
	FreeMemory(&scene);

	glfwDestroyWindow(window.frame);
	glfwTerminate();

	return 0;
}

int RenderLoop(Window *window, Input *input, Camera *camera, Scene *scene) {
	printf("Opened window, press ESC to exit\n");
	printf("View available commands with 'help'\n");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	struct Regex regex;

	SetNonBlocking();
	CreateRegexPatterns(&regex);
	
	mat4 modelTransform, viewTransform, projectionTransform;

	vec3 skyColor = {0.08, 0.07, 0.1};

	float deltaTime = 0.0;
	float currentFrame = 0.0, lastFrame = 0.0;

	unsigned int *shader;
	Model *model;
	Light *light;
	while(!glfwWindowShouldClose(window->frame)) {	
		scene->models[1].rotationAngle += 0.01;
		ParseInput(input, &regex, scene);

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetFramebufferSize(window->frame, &window->width, &window->height);
		ProcessKeyInput(window, camera, deltaTime);

		glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_mat4_identity(projectionTransform);
		glm_perspective(glm_rad(camera->zoom),
			(float)window->width/(float)window->height, 0.1, 100.0, projectionTransform
		);

		glm_mat4_identity(viewTransform);
		glm_vec3_add(camera->position, camera->front, camera->target);
		glm_lookat(camera->position, camera->target, (vec3){0.0, 1.0, 0.0}, viewTransform);

		for (unsigned int m = 0; m < scene->mCount; m++) {
			model = &scene->models[m];
			shader = &model->shader;
			glUseProgram(*shader);

			glm_mat4_identity(modelTransform);
			glm_translate(modelTransform, model->position);
			glm_rotate(modelTransform, model->rotationAngle, model->rotation);
			glm_scale(modelTransform, model->scale);

			ShaderSetMat4(shader, "projection", GL_FALSE, (float*) projectionTransform);
			ShaderSetMat4(shader, "view", GL_FALSE, (float*) viewTransform);
			ShaderSetMat4(shader, "model", GL_FALSE, (float*) modelTransform);

			UpdateModelShader(model, scene, camera);

			DrawModel(model);
		}

		for (unsigned int l = 0; l < scene->lCount; l++) {
			light = &scene->lights[l];
			shader = &light->shader;
			glUseProgram(*shader);

			glm_mat4_identity(modelTransform);
			glm_translate(modelTransform, light->position);

			ShaderSetMat4(shader, "projection", GL_FALSE, (float*) projectionTransform);
			ShaderSetMat4(shader, "view", GL_FALSE, (float*) viewTransform);
			ShaderSetMat4(shader, "model", GL_FALSE, (float*) modelTransform);

			ShaderSetVec3(shader, "light.position", &light->position);
			ShaderSetVec3(shader, "light.color", &light->color);

			glBindVertexArray(scene->debugPointVAO);
			glDrawArrays(GL_POINTS, 0, 1);
		}

		shader = &scene->floorShader;
		glUseProgram(*shader);

		ShaderSetMat4(shader, "projection", GL_FALSE, (float*) projectionTransform);
		ShaderSetMat4(shader, "view", GL_FALSE, (float*) viewTransform);

		ShaderSetVec3(shader, "camPos", &camera->position);

		glBindVertexArray(scene->debugPointVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		
		glfwSwapBuffers(window->frame);
		glfwPollEvents();
	}

	printf("Closed window\n");
	printf("Exiting program...\n");

	FreeRegexPatterns(&regex);

	return 0;
}

void FreeMemory(Scene *scene) {
	if (scene->models) {
		for (unsigned int i = 0; i < scene->mCount; i++) {
			FreeModel(&scene->models[i]);
		}

		free(scene->models);
	}
	if (scene->lights) {
		free(scene->lights);
	}
}
void FreeModel(Model *model) {
	if (model->meshes) {
		for (unsigned int i = 0; i < model->mCount; i++) {
			FreeMesh(&model->meshes[i]);
		}

		free(model->meshes);
	}
	if (model->savedTextures) {
		free(model->savedTextures);
	}
}
void FreeMesh(Mesh *mesh) {
	if (mesh->vertices) {
		free(mesh->vertices);
	}
	if (mesh->textures) {
		free(mesh->textures);
	}
	if (mesh->indices) {
		free(mesh->indices);
	}
}

void UpdateLight(Light *light) {
	glm_vec3_copy(light->color, light->specular);
	glm_vec3_mul(light->color, (vec3){0.8, 0.8, 0.8}, light->diffuse);
	glm_vec3_mul(light->color, (vec3){0.1, 0.1, 0.1}, light->ambient);
}

void CreateDebugPoint(Scene *scene) {
	glGenVertexArrays(1, &scene->debugPointVAO);
	glGenBuffers(1, &scene->debugPointVBO);

	glBindVertexArray(scene->debugPointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, scene->debugPointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), (vec3){0.0, 0.0, 0.0}, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
}

void InitializeStructs(Window *window, Input *input, Camera* camera, Mouse *mouse, Controls *controls, Scene *scene) {
	window->delay = glfwGetTime();
	window->width = INIT_WIDTH;
	window->height = INIT_HEIGHT;
	window->frame = NULL;

	input->opts = 0;

	glm_vec3_copy((vec3){0.0, 5.0, 5.0}, camera->position);
	glm_vec3_copy((vec3){0.0, -0.707, -0.707}, camera->front);
	camera->yaw = -1.57;
	camera->pitch = -0.785;
	camera->zoom = 90;

	mouse->lastX = (float)INIT_WIDTH / 2;
	mouse->lastY = (float)INIT_HEIGHT / 2;
	mouse->firstMouse = true;

	controls->camera = camera;
	controls->mouse = mouse;

	scene->mSelected = -1;
	scene->mCount = 4;
	scene->models = malloc(sizeof(Model) * scene->mCount);

	scene->lCount = 2;
	scene->lights = malloc(sizeof(Light) * scene->lCount);

	Light *light;
	for (unsigned int l = 0; l < scene->lCount; l++) {
		light = &scene->lights[l];

		glm_vec3_copy((vec3){0.0, 0.0, 0.0}, light->position);
		glm_vec3_copy((vec3){0.0, 0.0, 0.0}, light->rotation);
		glm_vec3_copy((vec3){0.0, 0.0, 0.0}, light->scale);

		glm_vec3_copy((vec3){1.0, 1.0, 1.0}, light->color);
		UpdateLight(light);

		light->attLinear = 0.001;
		light->attQuadratic = 0.01;
	}

	glm_vec3_copy((vec3){0.0, 5.0, 5.0}, scene->lights[0].position);
	glm_vec3_copy((vec3){1.0, 0.9, 0.8}, scene->lights[0].color);
	UpdateLight(&scene->lights[0]);
	glm_vec3_copy((vec3){4.0, 3.0, 1.0}, scene->lights[1].position);
	glm_vec3_copy((vec3){0.8, 0.9, 1.0}, scene->lights[1].color);
	UpdateLight(&scene->lights[1]);
}

void *ResizeArray(void *array, unsigned int size) {
	void *temp = realloc(array, size);
	if (!temp) return 0;
	return temp;
}
