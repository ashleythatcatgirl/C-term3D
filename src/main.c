
#include "main.h"

#include "../include/glad.c"

#include "mesh.h"
#include "parseInput.c"
#include "controls.c"
#include "shader.h"
#include "window.c"
#include "mesh.c"

#include "verticies.c"
#include <stdio.h>
#include <stdlib.h>

void InitializeStructs(Window *window, Input *input, Models *models, Textures *textures, Camera* camera, Mouse *mouse, Controls *controls, Scene *scene);
int RenderLoop(Window *window, Input *input, Models *models, Textures *textures, Camera *camera, Scene *scene);

int main() {
	struct Window window;
	struct Input input;

	struct Textures textures;
	struct Models models;

	struct Camera camera;
	struct Mouse mouse;
	struct Controls controls;

	struct Scene scene;
	
	InitializeStructs(&window, &input, &models, &textures, &camera, &mouse, &controls, &scene);

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
	printf("loading model 1\n");
	stbi_set_flip_vertically_on_load(false);
	LoadModel(&scene.models[1], "../models/tannhauser/tannhauser.obj");

	LoadShader(&scene.models[0].shader, "shaders/vertex/normal.glsl", "shaders/fragment/object.glsl");
	LoadShader(&scene.models[1].shader, "shaders/vertex/normal.glsl", "shaders/fragment/object.glsl");

	printf("Compiling shaders..\n");
	for (int obj = 0; obj < models.count; obj++) {
		Model2 *model = &models.model[obj];
		if (model->type == OBJ_MODEL)
			LoadShader(&model->shader, "shaders/vertex/normal.glsl", "shaders/fragment/object.glsl");
		else if (model->type == OBJ_LIGHT_POINT)
			LoadShader(&model->shader, "shaders/vertex/normal.glsl", "shaders/fragment/light.glsl");

		SetModelData(model);
		UpdateShaderUniform(&model->shader, &models, model, &camera);
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
		goto exitProgram;
	}

	printf("Loading models\n");

	printf("Loading successful, press enter to continue..");
	getchar();

	RenderLoop(&window, &input, &models, &textures, &camera, &scene);

exitProgram:
	FreeMemory(&models, &textures);

	glfwDestroyWindow(window.frame);
	glfwTerminate();

	return 0;
}

int RenderLoop(Window *window, Input *input, Models *models, Textures *textures, Camera *camera, Scene *scene) {
	printf("Opened window, press ESC to exit\n");
	printf("View available commands with 'help'\n");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	struct Regex regex;

	SetNonBlocking();
	CreateRegexPatterns(&regex);
	
	mat4 modelTransform, viewTransform, projectionTransform;

	vec3 skyColor = {0.5, 0.5, 0.6};

	float deltaTime = 0.0;
	float currentFrame = 0.0, lastFrame = 0.0;
	while(!glfwWindowShouldClose(window->frame)) {	
		ParseInput(input, &regex, models, textures);

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetFramebufferSize(window->frame, &window->width, &window->height);
		ProcessKeyInput(window, camera, deltaTime);

		glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_mat4_identity(viewTransform);
		glm_vec3_add(camera->position, camera->front, camera->target);
		glm_lookat(camera->position, camera->target, (vec3){0.0, 1.0, 0.0}, viewTransform);

		glm_mat4_identity(projectionTransform);
		glm_perspective(glm_rad(camera->zoom), (float)window->width/(float)window->height, 0.1, 100.0, projectionTransform);


		for (unsigned int m = 0; m < scene->mCount; m++) {
			glUseProgram(*&scene->models[m].shader);

			glm_mat4_identity(modelTransform);
			if (m == 1) {
				glm_translate(modelTransform, (vec3){-4.0, -2.9, 0.0});
				glm_scale(modelTransform, (vec3){0.05, 0.05, 0.05});
			} else if (m == 0) {
				glm_translate(modelTransform, (vec3){4.0, -1.2, 0.0});
			}

			ShaderSetMat4(&scene->models[m].shader, "view", GL_FALSE, (float*) viewTransform);
			ShaderSetMat4(&scene->models[m].shader, "projection", GL_FALSE, (float*) projectionTransform);
			ShaderSetMat4(&scene->models[m].shader, "model", GL_FALSE, (float*) modelTransform);

			float c = 16;
			ShaderSetFloat(&scene->models[m].shader, "material.shininess", &c);

			ShaderSetVec3(&scene->models[m].shader, "light[0].position", &models->model[12].translate);
			ShaderSetVec3(&scene->models[m].shader, "light[0].ambient", &models->model[12].data.light.ambient);
			ShaderSetVec3(&scene->models[m].shader, "light[0].diffuse", &models->model[12].data.light.diffuse);
			ShaderSetVec3(&scene->models[m].shader, "light[0].specular", &models->model[12].data.light.specular);
			ShaderSetFloat(&scene->models[m].shader, "light[0].attLinear", &models->model[12].data.light.attLinear);
			ShaderSetFloat(&scene->models[m].shader, "light[0].attQuadratic", &models->model[12].data.light.attQuadratic);

			ShaderSetVec3(&scene->models[m].shader, "light[1].position", &models->model[13].translate);
			ShaderSetVec3(&scene->models[m].shader, "light[1].ambient", &models->model[13].data.light.ambient);
			ShaderSetVec3(&scene->models[m].shader, "light[1].diffuse", &models->model[13].data.light.diffuse);
			ShaderSetVec3(&scene->models[m].shader, "light[1].specular", &models->model[13].data.light.specular);
			ShaderSetFloat(&scene->models[m].shader, "light[1].attLinear", &models->model[13].data.light.attLinear);
			ShaderSetFloat(&scene->models[m].shader, "light[1].attQuadratic", &models->model[13].data.light.attQuadratic);

			ShaderSetVec3(&scene->models[m].shader, "light[2].position", &models->model[14].translate);
			ShaderSetVec3(&scene->models[m].shader, "light[2].ambient", &models->model[14].data.light.ambient);
			ShaderSetVec3(&scene->models[m].shader, "light[2].diffuse", &models->model[14].data.light.diffuse);
			ShaderSetVec3(&scene->models[m].shader, "light[2].specular", &models->model[14].data.light.specular);
			ShaderSetFloat(&scene->models[m].shader, "light[2].attLinear", &models->model[14].data.light.attLinear);
			ShaderSetFloat(&scene->models[m].shader, "light[2].attQuadratic", &models->model[14].data.light.attQuadratic);

			ShaderSetVec3(&scene->models[m].shader, "camPos", &camera->position);

			DrawModel(&scene->models[m]);
		}
		
		for (int obj = 0; obj < models->count; obj++) {
			Model2 *model = &models->model[obj];

			if (models->model[obj].type == OBJ_MODEL) {
				glActiveTexture(GL_TEXTURE0);
				if (textures->texture[model->data.material.texture].diffuse != NULL) {
					glBindTexture(GL_TEXTURE_2D, textures->texture[model->data.material.texture].diffuse[0].memory);
				}
				glActiveTexture(GL_TEXTURE1);
				if (textures->texture[model->data.material.texture].specular != NULL) {
					glBindTexture(GL_TEXTURE_2D, textures->texture[model->data.material.texture].specular[0].memory);
				}
			}

			glUseProgram(model->shader);

			ShaderSetMat4(&model->shader, "view", GL_FALSE, (float*) viewTransform);
			ShaderSetMat4(&model->shader, "projection", GL_FALSE, (float*) projectionTransform);

			glBindVertexArray(model->VAO);
			glm_mat4_identity(modelTransform);
			glm_translate(modelTransform, model->translate);
			for (int i = 0; i < 3; i++) {
				if (model->rotate[i] == 0) continue;

				glm_rotate(modelTransform, obj < 10 ? glfwGetTime() : 1.57, model->rotate);
				break;
			}
			glm_scale(modelTransform, model->scale);
			ShaderSetMat4(&model->shader, "model", GL_FALSE, (float*) modelTransform);

			UpdateShaderUniform(&model->shader, models, model, camera);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	
		glfwSwapBuffers(window->frame);
		glfwPollEvents();
	}

	printf("Closed window\n");
	printf("Exiting program...\n");

	FreeRegexPatterns(&regex);

	return 0;
}

void SetModelData(Model2 *model) {
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
	struct dirent *de1, *de2;
	DIR *dr1 = NULL, *dr2 = NULL;
	
	if ((dr1 = opendir(TEXTURE_DIRECTORY)) == NULL) return 1;

	textures->texture = (Texture2*)malloc(sizeof(Texture2));
	if (textures->texture == NULL) return -1;

	char subDir[64], textureName[64];
	char *end = NULL;
	int a = 0;
	bool diffuse = false, specular = false;
	Texture2 *texture = NULL;
	while ((de1 = readdir(dr1)) != NULL) {
		if (de1->d_name[0] == '.') continue;
		printf("\n  ->Texture %s\n", de1->d_name);

		strcpy(subDir, "../files/textures/");
		strcat(subDir, de1->d_name);
		strcat(subDir, "/\0");

		if ((dr2 = opendir(subDir)) == NULL) continue;

		if (a == 1) {
			Texture2 *temp = NULL;
			temp = (Texture2*)realloc(textures->texture, sizeof(Texture2) * (textures->count + 1));
			if (temp == NULL) return -1;

			textures->texture = temp;
		}
		texture = &textures->texture[textures->count];
		texture->diffuseCount = 0;
		texture->specularCount = 0;
		texture->diffuse = NULL;
		texture->specular = NULL;

		strcpy(textureName, TEXTURE_DIRECTORY);
		strcat(textureName, de1->d_name);
		strcat(textureName, "/\0");

		a = 0;

		while ((de2 = readdir(dr2)) != NULL) {
			diffuse = false, specular = false;
			if (de2->d_name[0] == '.') continue;
			
			if ((end = strchr(de2->d_name, '.')) == NULL) continue;
			if (strcmp(end, ".png") && strcmp(end,".jpg")) continue;

			if (strstr(de2->d_name, "diffuse") != NULL) diffuse = true;
			if (strstr(de2->d_name, "specular") != NULL) specular = true;
			if (diffuse == false && specular == false) continue;

			printf("    ->%s\n", de2->d_name);

			if (diffuse == true) {
				texture->diffuseCount++;
				if (texture->diffuse == NULL) {
					texture->diffuse = malloc(sizeof(Tex));
					if (texture->diffuse == NULL) return -1;
				} else {
					Tex *temp = NULL;
					temp = realloc(texture->diffuse, sizeof(Tex) * texture->diffuseCount);
					if (temp == NULL) return -1;

					texture->diffuse = temp;
				}
				strcpy(texture->diffuse[0].name, textureName);
				strcat(texture->diffuse[0].name, de2->d_name);
			} else if (specular == true) {
				texture->specularCount++;
				if (texture->specular == NULL) {
					texture->specular = malloc(sizeof(Tex));
					if (texture->specular == NULL) return -1;
				} else {
					Tex *temp = NULL;
					temp = realloc(texture->specular, sizeof(Tex) * texture->specularCount);
					if (temp == NULL) return -1;

					texture->specular = temp;

				}
				strcpy(texture->specular[0].name, textureName);
				strcat(texture->specular[0].name, de2->d_name);
			}

			a = 1;
		}
		closedir(dr2);

		if (a == 0) continue;
		if (texture->diffuse == NULL) {
			printf("No diffuse map found\n");
			return -1;
		}
		if (texture->specular == NULL) {
			printf("No specular map found\n");
		}

		textures->count++;
	}
	closedir(dr1);

	printf("\n->Setting texture data..\n");
	for (int i = 0; i < textures->count; i++) {
		texture = &textures->texture[i];
		printf("  ->Texture[%d] diffuse\n", i);
		if (GenerateTexture(texture->diffuse) == -1) return -1;

		if (texture->specular == NULL) continue;

		printf("  ->Texture[%d] specular\n", i);
		if (GenerateTexture(texture->specular) == -1) return -1;
	}

	return 0;
}

int GenerateTexture(Tex *tex) {
	int textureWidth, textureHeight, colorChannels;
	unsigned char *data;

	glGenTextures(1, &tex[0].memory);
	glBindTexture(GL_TEXTURE_2D, tex[0].memory);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load(tex[0].name, &textureWidth, &textureHeight, &colorChannels, 4);
	if (!data) return -1;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

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
		for (int tex = 0; tex < textures->count; tex++) {
			if (textures->texture[tex].diffuse != NULL)
				free(textures->texture[tex].diffuse);
			if (textures->texture[tex].specular != NULL)
				free(textures->texture[tex].specular);
			
		}

		free(textures->texture);
	}
}


void UpdateLight(Model2 *light) {
	glm_vec3_copy(light->data.light.color, light->data.light.specular);
	glm_vec3_mul(light->data.light.color, (vec3){0.8, 0.8, 0.8}, light->data.light.diffuse);
	glm_vec3_mul(light->data.light.color, (vec3){0.1, 0.1, 0.1}, light->data.light.ambient);
}

void InitializeStructs(Window *window, Input *input, Models *models, Textures *textures, Camera* camera, Mouse *mouse, Controls *controls, Scene *scene) {
	window->delay = glfwGetTime();
	window->width = INIT_WIDTH;
	window->height = INIT_HEIGHT;
	window->frame = NULL;

	input->opts = 0;

	textures->count = 0;
	textures->texture = NULL;

	models->count = 15;
	models->model = malloc(sizeof(Model2) * models->count);

	vec3 cubeT[10] = {
		{-2.0, 1.0, 0.0},
		{2.0, 1.0, -1.0},
		{2.0, 1.0, 2.0},
		{-3.0, -2.0, 0.0},
		{0.0, -1.5, 0.0},
		{4.0, 0.0, 1.0},
		{3.0, -1.0, -2.0},
		{-1.0, 4.0, -3.0},
		{8.0, 2.0, -4.0},
		{-7.0, 4.0, 1.0}
	};
	vec3 cubeR[10] = {
		{-1.0, 1.0, 0.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, 1.0},
		{-1.0, -1.0, 0.0},
		{0.0, -1.0, 0.0},
		{1.0, 0.0, 1.0},
		{1.0, -1.0, -1.0},
		{-1.0, 1.0, -1.0},
		{-1.0, 1.0, -1.0},
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

	vec3 lightT[3] = {
		{-3.5, 5.0, 2.0},
		{1.0, 3.0, -4.0},
		{6.0, 2.0, -1.0}
	};
	vec3 lightR = {0.0, 0.0, 1.0};
	vec3 lightS = {0.5, 0.5, 0.5};

	vec3 lightColor[3] = {
		{0.9, 0.5, 1.0},
		{1.0, 0.9, 0.5},
		{0.5, 1.0, 0.9}
	};

	for (int o = 0; o < 10; o++) {
		models->model[o].type = OBJ_MODEL;
		models->model[o].data.material.texture = 0;
		models->model[o].data.material.shininess = 64;

		glm_vec3_copy(cubeT[o], models->model[o].translate);
		glm_vec3_copy(cubeR[o], models->model[o].rotate);
		glm_vec3_normalize(models->model[o].rotate);
		glm_vec3_copy(cubeS, models->model[o].scale);
	}
	for (int o = 10; o < 12; o++) {
		models->model[o].type = OBJ_MODEL;
		models->model[o].data.material.texture = 2;
		models->model[o].data.material.shininess = 64;

		glm_vec3_copy(floorT[o - 10], models->model[o].translate);
		glm_vec3_copy(floorR[o - 10], models->model[o].rotate);
		glm_vec3_normalize(models->model[o].rotate);
		glm_vec3_copy(floorS, models->model[o].scale);
	}
	for (int o = 12; o < 15; o++) {
		models->model[o].type = OBJ_LIGHT_POINT;

		glm_vec3_copy(lightT[o - 12], models->model[o].translate);
		glm_vec3_copy(lightR, models->model[o].rotate);
		glm_vec3_normalize(models->model[o].rotate);
		glm_vec3_copy(lightS, models->model[o].scale);

		glm_vec3_copy(lightColor[o - 12], models->model[o].data.light.color);
		UpdateLight(&models->model[o]);
		models->model[o].data.light.attLinear = 0.001;
		models->model[o].data.light.attQuadratic = 0.01;
	}

	glm_vec3_copy((vec3){0.0, 0.0, 3.0}, camera->position);
	glm_vec3_copy((vec3){0.0, 0.0, -1.0}, camera->front);
	camera->yaw = -1.57;
	camera->pitch = 0.0;
	camera->zoom = 90;

	mouse->lastX = (float)INIT_WIDTH / 2;
	mouse->lastY = (float)INIT_HEIGHT / 2;
	mouse->firstMouse = true;

	controls->camera = camera;
	controls->mouse = mouse;

	scene->mCount = 2;
	scene->models = malloc(sizeof(Model) * scene->mCount);
}

void *ResizeArray(void *array, unsigned int size) {
	void *temp = realloc(array, size);
	if (!temp) return 0;
	return temp;
}
