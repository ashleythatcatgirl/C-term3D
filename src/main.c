
#include "main.h"

#include "include/glad.c"

#include "shader.c"
#include "camera.c"
#include "parseInput.c"

#include "verticies.c"
#include <GLFW/glfw3.h>

int main(int argc, char **argv) {
	struct Window window;
	struct Input input;

	struct Textures textures;
	struct Models models;
	struct Transforms transforms;

	struct Camera camera;
	struct Mouse mouse;
	struct Controls controls;
	
	InitializeStructs(&window, &input, &textures, &models, &transforms, &camera, &mouse, &controls);

	switch(CreateWindow(&window, &controls)) {
	case 0:
		printf("Window created successfully\n");
		break;
	case 1:
		printf("Window failed to create\n");
		glfwTerminate();
		break;
	}

	printf("Initializing object vertex data..\n");
	for (int obj = 0; obj < models.count; obj++) {
		Model *model = &models.model[obj];
		if (model->type == OBJ_MODEL) LoadShader(&model->shader, "shaders/vertex/normal1.glsl", "shaders/fragment/object.glsl");
		else if (model->type == OBJ_LIGHT) LoadShader(&model->shader, "shaders/vertex/normal1.glsl", "shaders/fragment/light.glsl");

		SetModelData(model);
		UpdateShaderUniform(&model->shader, model, &models.model[2], &camera);
	}

	switch (LoadTextures(&textures)) {
	case 0:
		printf("\nTextures loaded\n");
		break;
	case 1:
		printf("\nNo textures found\n");
		break;
	case -1:
		printf("\nTexture loading failed\n");
		glfwTerminate();
		return 1;
	}

	for (int i = 0; i < 2; i++) {
		printf("Model %d textures\n", i);
		LinkTextures(&textures, &models.model[i].shader);
	}

	printf("Loading successful, press enter to continue..");
	getchar();

	RenderLoop(&window, &input, &models, &textures, &transforms, &camera);
	FreeMemory(&models, &textures);

	glfwTerminate();

	return 0;
}

int RenderLoop(Window *window, Input *input, Models *models, Textures *textures, Transforms *transforms, Camera *camera) {
	printf("Opened window, press ESC to exit\n");
	printf("View available commands with 'help'\n");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetNonBlocking();
	
	vec3 skyColor = {0.5, 0.5, 0.6};

	float deltaTime = 0.0;
	float lastFrame = 0.0;
	float currentFrame = 0.0;
	while(!glfwWindowShouldClose(window->frame)) {	
		ParseInput(input, models, textures);

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetFramebufferSize(window->frame, &window->width, &window->height);
		ProcessKeyInput(window, camera, deltaTime);

		glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_mat4_identity(transforms->view);
		glm_vec3_add(camera->position, camera->front, camera->target);
		glm_lookat(camera->position, camera->target, (vec3){0.0, 1.0, 0.0}, transforms->view);

		glm_mat4_identity(transforms->projection);
		glm_perspective(glm_rad(camera->zoom), (float)window->width/(float)window->height, 0.1, 100.0, transforms->projection);

		for (int obj = 0; obj < models->count; obj++) {
			Model *model = &models->model[obj];
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures->texture[model->material.texture].memory);

			glUseProgram(model->shader);

			ShaderSetMat4(&model->shader, "view", GL_FALSE, (float*) transforms->view);
			ShaderSetMat4(&model->shader, "projection", GL_FALSE, (float*) transforms->projection);

			glBindVertexArray(model->VAO);
			for (int tr = 0; tr < model->transformCount; tr++) {
				glm_mat4_identity(transforms->model);

				/*
				glm_vec3_copy((vec3){fabs(sin(glfwGetTime())), fabs(sin(glfwGetTime() - 1.05)), fabs(sin(glfwGetTime() - 2.10))}, models->model[2].light.color);
				glm_vec3_copy(models->model[2].light.color, models->model[2].light.specular);
				glm_vec3_mul(models->model[2].light.color, (vec3){0.9, 0.9, 0.9}, models->model[2].light.diffuse);
				glm_vec3_mul(models->model[2].light.diffuse, (vec3){0.9, 0.9, 0.9}, models->model[2].light.ambient);
				*/

				UpdateShaderUniform(&model->shader, model, &models->model[2], camera);

				glm_translate(transforms->model, model->translate[tr]);
				for (int i = 0; i < 3; i++) {
					if (model->rotate[tr][i] == 0) continue;

					glm_rotate(transforms->model, obj == 0 ? glfwGetTime() : 1.57, model->rotate[tr]);
					break;
				}
				glm_scale(transforms->model, model->scale);

				ShaderSetMat4(&model->shader, "model", GL_FALSE, (float*) transforms->model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	
		glfwSwapBuffers(window->frame);
		glfwPollEvents();
	}

	printf("Closed window\n");
	printf("Exiting program...\n");

	return 0;
}

void ProcessKeyInput(Window *window, Camera *camera, float deltaTime) {
	if(glfwGetKey(window->frame, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window->frame, true);
	}
	if(glfwGetKey(window->frame, GLFW_KEY_TAB) == GLFW_PRESS
	&& window->delay < glfwGetTime() - 0.25) {
		window->delay = glfwGetTime();
		glfwSetInputMode(window->frame, GLFW_CURSOR,
		glfwGetInputMode(window->frame, GLFW_CURSOR) == GLFW_CURSOR_DISABLED?
		GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	float moveSpeed = camera->moveSpeed * deltaTime;
	float turnSpeed = camera->turnSpeed * deltaTime;
	if(glfwGetKey(window->frame, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveSpeed *= 4;

	if(glfwGetKey(window->frame, GLFW_KEY_W) == GLFW_PRESS) CameraMoveZ(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_S) == GLFW_PRESS) CameraMoveZ(camera, -moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_A) == GLFW_PRESS) CameraMoveX(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_D) == GLFW_PRESS) CameraMoveX(camera, -moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_E) == GLFW_PRESS) CameraMoveY(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_Q) == GLFW_PRESS) CameraMoveY(camera, -moveSpeed);

	if(glfwGetKey(window->frame, GLFW_KEY_RIGHT) == GLFW_PRESS) CameraYaw(camera, turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_LEFT) == GLFW_PRESS) CameraYaw(camera, -turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_UP) == GLFW_PRESS) CameraPitch(camera, turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_DOWN) == GLFW_PRESS) CameraPitch(camera, -turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_C) == GLFW_PRESS) CameraZoom(camera, 1);
	if(glfwGetKey(window->frame, GLFW_KEY_Z) == GLFW_PRESS) CameraZoom(camera, -1);
}


void SetModelData(Model *model) {
	glGenBuffers(1, &model->VBO);
	glGenVertexArrays(1, &model->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	glBindVertexArray(model->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
}

int LoadTextures(Textures *textures) {
	printf("Initializing texture data..\n");

	printf("\n->Reading texture data..\n");
	struct dirent *de;
	DIR *dr = opendir(TEXTURE_DIRECTORY);
	if (dr == NULL) return 1;
	printf("  ->Found texture folder!\n");

	char *end = NULL;
	while ((de = readdir(dr)) != NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;

		if ((end = strchr(de->d_name, '.')) == NULL) continue;
		if (strcmp(end, ".png") && strcmp(end,".jpg")) continue;

		textures->count++;
	}

	closedir(dr);

	if (textures->count == 0) return 1;

	textures->texture = (Texture*)malloc(sizeof(Texture) * textures->count);
	if (textures->texture == NULL) return -1;

	dr = opendir(TEXTURE_DIRECTORY);

	for (int tex = 0; (de = readdir(dr)) != NULL;) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;

		if ((end = strchr(de->d_name, '.')) == NULL) continue;
		if (strcmp(end, ".png") && strcmp(end,".jpg")) continue;

		strcpy(textures->texture[tex].name, TEXTURE_DIRECTORY);
		strcat(textures->texture[tex].name, de->d_name);
		printf("    ->Texture[%d]: %s\n", tex, textures->texture[tex].name);
		tex++;
	}

	closedir(dr);

	unsigned char *data;
	int textureWidth, textureHeight, colorChannels;

	stbi_set_flip_vertically_on_load(true);
	printf("\n->Setting texture data..\n");
	for (int i = 0; i < textures->count; i++) {
		printf("  ->Texture[%d]\n", i);
		glGenTextures(1, &textures->texture[i].memory);
		glBindTexture(GL_TEXTURE_2D, textures->texture[i].memory);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(textures->texture[i].name, &textureWidth, &textureHeight, &colorChannels, 4);
		if (!data) return -1;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	return 0;
}

int LinkTextures(Textures *textures, unsigned int *shaderProgram) {
	printf("\n->Linking texture data..\n");
	glUseProgram(*shaderProgram);

	glUniform1i(glGetUniformLocation(*shaderProgram, "texture1"), 0);

	return 0;
}

void FreeMemory(Models *models, Textures *textures) {
	if (models->model != NULL) {
		for (int obj = 0; obj < models->count; obj++) {
 	   		glDeleteBuffers(1, &models->model[obj].VBO);
			glDeleteVertexArrays(1, &models->model[obj].VAO);

    			glDeleteProgram(models->model[obj].shader);
		}

		free(models->model);
	}
	if (textures->texture != NULL) {
		free(textures->texture);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
} 

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	Controls *controls = glfwGetWindowUserPointer(window);
	if (controls->mouse->firstMouse) {
		controls->mouse->lastX = xPos;
		controls->mouse->lastY = yPos;

		controls->mouse->firstMouse = false;
	}

	controls->mouse->xOffset = xPos - controls->mouse->lastX;
	controls->mouse->yOffset = controls->mouse->lastY - yPos;
	controls->mouse->xOffset *= controls->mouse->sensitivity;
	controls->mouse->yOffset *= controls->mouse->sensitivity;

	controls->mouse->lastX = xPos;
	controls->mouse->lastY = yPos;
	
	CameraYaw(controls->camera, controls->mouse->xOffset, window);
	CameraPitch(controls->camera, controls->mouse->yOffset, window);
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
	Controls *controls = glfwGetWindowUserPointer(window);

	CameraZoom(controls->camera, 2*yOffset);
}

int CreateWindow(Window *window, Controls *controls) {
	printf("Window initializiation..\n");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window->frame = glfwCreateWindow(window->width, window->height, "C-term3D", NULL, NULL);
	if (window->frame == NULL) {
		printf("Window creation error\n");
		return 1;
	}

	glfwSetWindowUserPointer(window->frame, (void*)controls);

	glfwMakeContextCurrent(window->frame);
	glfwSetFramebufferSizeCallback(window->frame, framebuffer_size_callback);  
	glfwSetCursorPosCallback(window->frame, mouse_callback);  
	glfwSetScrollCallback(window->frame, scroll_callback);

	glfwSetInputMode(window->frame, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("GLAD error\n");
		return 1;
	}

	return 0;
}

void InitializeStructs(Window *window, Input *input, Textures *textures, Models *models, Transforms *transforms, Camera* camera, Mouse *mouse, Controls *controls) {
	window->delay = glfwGetTime();
	window->width = INIT_WIDTH;
	window->height = INIT_HEIGHT;
	window->frame = NULL;

	input->opts = 0;

	textures->count = 0;
	textures->texture = NULL;

	models->count = 3;
	models->model = malloc(sizeof(Model) * models->count);

	vec3 cubeT[8] = {
		{-2.0, 1.0, 0.0},
		{2.0, 1.0, -1.0},
		{2.0, 1.0, 2.0},
		{-3.0, -2.0, 0.0},
		{0.0, -1.5, 0.0},
		{4.0, 0.0, 1.0},
		{3.0, -1.0, -2.0},
		{-1.0, 4.0, -3.0}
	};
	vec3 cubeR[8] = {
		{-1.0, 1.0, 0.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, 1.0},
		{-1.0, -1.0, 0.0},
		{0.0, -1.0, 0.0},
		{1.0, 0.0, 1.0},
		{1.0, -1.0, -1.0},
		{-1.0, 1.0, -1.0}
	};
	vec3 cubeS = {1.0, 1.0, 1.0};

	vec3 floorT[2] = {
		{0.0, -3.0, 0.0},
		{0.0, 7.0, -10.0}
	};
	vec3 floorR[2] = {
		{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0}
	};
	vec3 floorS = {20.0, 0.2, 20.0};

	vec3 lightT = {0.0, 15.0, 0.0};
	vec3 lightR = {0.0, 0.0, 1.0};
	vec3 lightS = {0.5, 0.5, 0.5};

	// MODEL 0
	models->model[0].type = OBJ_MODEL;
	models->model[1].type = OBJ_MODEL;
	models->model[2].type = OBJ_LIGHT;

	models->model[0].transformCount = 8;
	models->model[1].transformCount = 2;
	models->model[2].transformCount = 1;

	models->model[0].material.texture = 0;
	models->model[1].material.texture = 1;

	glm_vec3_copy((vec3){0.05, 0.045, 0.04}, models->model[0].material.ambient);
	glm_vec3_copy((vec3){0.4, 0.3, 0.25}, models->model[0].material.diffuse);
	glm_vec3_copy((vec3){0.1, 0.1, 0.1}, models->model[0].material.specular);
	models->model[0].material.shininess = 2;
	glm_vec3_copy((vec3){0.9, 0.7, 0.3}, models->model[0].material.color);

	for (int obj = 0; obj < models->count; obj++) {
		models->model[obj].translate = malloc(sizeof(vec3) * models->model[obj].transformCount);
		models->model[obj].rotate = malloc(sizeof(vec3) * models->model[obj].transformCount);
	}

	for (int tr = 0; tr < models->model[0].transformCount; tr++) {
		glm_vec3_copy(cubeT[tr], models->model[0].translate[tr]);

		glm_vec3_copy(cubeR[tr], models->model[0].rotate[tr]);
		glm_vec3_normalize(models->model[0].rotate[tr]);
	}
	glm_vec3_copy(cubeS, models->model[0].scale);

	// MODEL 1
	glm_vec3_copy((vec3){0.1, 0.15, 0.2}, models->model[1].material.ambient);
	glm_vec3_copy((vec3){0.4, 0.5, 0.55}, models->model[1].material.diffuse);
	glm_vec3_copy((vec3){0.6, 0.6, 0.6}, models->model[1].material.specular);
	models->model[1].material.shininess = 128;
	glm_vec3_copy((vec3){0.5, 0.6, 0.4}, models->model[1].material.color);

	for (int tr = 0; tr < models->model[1].transformCount; tr++) {
		glm_vec3_copy(floorT[tr], models->model[1].translate[tr]);

		glm_vec3_copy(floorR[tr], models->model[1].rotate[tr]);
		glm_vec3_normalize(models->model[1].rotate[tr]);
	}
	glm_vec3_copy(floorS, models->model[1].scale);

	// MODEL 2
	glm_vec3_copy((vec3){1.0, 1.0, 1.0}, models->model[2].light.color);

	glm_vec3_copy(models->model[2].light.color, models->model[2].light.specular);
	glm_vec3_mul(models->model[2].light.color, (vec3){0.9, 0.9, 0.9}, models->model[2].light.diffuse);
	glm_vec3_mul(models->model[2].light.diffuse, (vec3){0.3, 0.3, 0.3}, models->model[2].light.ambient);

	glm_vec3_copy(lightT, models->model[2].translate[0]);

	glm_vec3_copy(lightR, models->model[2].rotate[0]);
	glm_vec3_normalize(models->model[2].rotate[0]);

	glm_vec3_copy(lightS, models->model[2].scale);

	glm_vec3_copy((vec3){0.0, 0.0, 3.0}, camera->position);
	glm_vec3_copy((vec3){0.0, 0.0, -1.0}, camera->front);
	camera->turnSpeed = 2.5;
	camera->moveSpeed = 2.5;
	camera->yaw = -1.57;
	camera->pitch = 0.0;
	camera->zoom = 90;

	mouse->lastX = (float)INIT_WIDTH / 2;
	mouse->lastY = (float)INIT_HEIGHT / 2;
	mouse->xOffset = 0;
	mouse->yOffset = 0;
	mouse->sensitivity = 0.01;
	mouse->firstMouse = true;

	controls->camera = camera;
	controls->mouse = mouse;
}
