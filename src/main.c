
#include "main.h"

#include "include/cglm/vec3.h"
#include "include/glad.c"

#include "parseInput.h"
#include "shader.c"
#include "parseInput.c"
#include "controls.c"
#include "window.c"

#include "verticies.c"

#include <GLFW/glfw3.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	struct Window window;
	struct Input input;
	struct Regex regex;

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
		goto exitProgram;
	}

	printf("Initializing object vertex data..\n");
	for (int obj = 0; obj < models.count; obj++) {
		Model *model = &models.model[obj];
		if (model->type == OBJ_MODEL)
			LoadShader(&model->shader, "shaders/vertex/normal1.glsl", "shaders/fragment/object.glsl");
		else if (model->type == OBJ_LIGHT_POINT)
			LoadShader(&model->shader, "shaders/vertex/normal1.glsl", "shaders/fragment/light.glsl");

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
		goto exitProgram;
	}

	/*for (int i = 0; i < 2; i++) {
		printf("Model %d textures\n", i);
		LinkTextures(&textures, &models.model[i].shader);
	}*/

	printf("Loading successful, press enter to continue..");
	getchar();

	RenderLoop(&window, &input, &regex, &models, &textures, &transforms, &camera);

exitProgram:
	FreeMemory(&models, &textures);
	FreeRegexPatterns(&regex);

	glfwDestroyWindow(window.frame);
	glfwTerminate();

	return 0;
}

int RenderLoop(Window *window, Input *input, Regex *regex, Models *models, Textures *textures, Transforms *transforms, Camera *camera) {
	printf("Opened window, press ESC to exit\n");
	printf("View available commands with 'help'\n");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetNonBlocking();
	CreateRegexPatterns(regex);
	
	vec3 skyColor = {0.5, 0.5, 0.6};

	float deltaTime = 0.0;
	float lastFrame = 0.0;
	float currentFrame = 0.0;
	while(!glfwWindowShouldClose(window->frame)) {	
		ParseInput(input, regex, models, textures);

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

		//glm_vec3_copy((vec3){fabs(sin(glfwGetTime())), fabs(sin(2 * glfwGetTime())), fabs(sin(3 * glfwGetTime()))}, models->model[2].data.light.color);
		//UpdateLight(&models->model[2]);

		for (int obj = 0; obj < models->count; obj++) {
			Model *model = &models->model[obj];

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

			ShaderSetMat4(&model->shader, "view", GL_FALSE, (float*) transforms->view);
			ShaderSetMat4(&model->shader, "projection", GL_FALSE, (float*) transforms->projection);

			glBindVertexArray(model->VAO);
			for (int tr = 0; tr < model->transformCount; tr++) {
				glm_mat4_identity(transforms->model);

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
	struct dirent *de1, *de2;
	DIR *dr1 = NULL, *dr2 = NULL;
	
	if ((dr1 = opendir(TEXTURE_DIRECTORY)) == NULL) return 1;

	textures->texture = (Texture*)malloc(sizeof(Texture) * 1);
	if (textures->texture == NULL) return -1;

	char subDir[64], textureName[64];
	char *end = NULL;
	int a = 0;
	bool diffuse = false, specular = false;
	Texture *texture = NULL;
	while ((de1 = readdir(dr1)) != NULL) {
		if (de1->d_name[0] == '.') continue;
		printf("\n  ->Texture %s\n", de1->d_name);

		strcpy(subDir, "../files/textures/");
		strcat(subDir, de1->d_name);
		strcat(subDir, "/\0");

		if ((dr2 = opendir(subDir)) == NULL) continue;

		if (a == 1) {
			Texture *temp = NULL;
			temp = (Texture*)realloc(textures->texture, sizeof(Texture) * (textures->count + 1));
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

	// end
	
	unsigned char *data;
	int textureWidth, textureHeight, colorChannels;

	stbi_set_flip_vertically_on_load(true);
	printf("\n->Setting texture data..\n");
	for (int i = 0; i < textures->count; i++) {
		texture = &textures->texture[i];
		printf("  ->Texture[%d] diffuse\n", i);
		glGenTextures(1, &texture->diffuse[0].memory);
		glBindTexture(GL_TEXTURE_2D, texture->diffuse[0].memory);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(texture->diffuse[0].name, &textureWidth, &textureHeight, &colorChannels, 4);
		if (!data) return -1;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		if (texture->specular != NULL) {
			printf("  ->Texture[%d] specular\n", i);
			glGenTextures(1, &texture->specular[0].memory);
			glBindTexture(GL_TEXTURE_2D, texture->specular[0].memory);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			data = stbi_load(texture->specular[0].name, &textureWidth, &textureHeight, &colorChannels, 4);
			if (!data) return -1;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		} else {
		}
	}

	return 0;
}

int LinkTextures(Textures *textures, unsigned int *shaderProgram) {
	/*printf("\n->Linking texture data..\n");
	glUseProgram(*shaderProgram);

	glUniform1i(glGetUniformLocation(*shaderProgram, "texture1"), 0);
	*/

	return 0;
}

void FreeMemory(Models *models, Textures *textures) {
	if (models->model != NULL) {
		for (int obj = 0; obj < models->count; obj++) {
			if (models->model[obj].translate != NULL)
				free(models->model[obj].translate);
			if (models->model[obj].rotate != NULL)
				free(models->model[obj].rotate);

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


void UpdateLight(Model *light) {
	glm_vec3_copy(light->data.light.color, light->data.light.specular);
	glm_vec3_mul(light->data.light.color, (vec3){0.8, 0.8, 0.8}, light->data.light.diffuse);
	glm_vec3_mul(light->data.light.color, (vec3){0.2, 0.2, 0.2}, light->data.light.ambient);
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

	vec3 lightT = {0.0, 15.0, 0.0};
	vec3 lightR = {0.0, 0.0, 1.0};
	vec3 lightS = {0.5, 0.5, 0.5};

	// MODEL 0
	models->model[0].type = OBJ_MODEL;
	models->model[1].type = OBJ_MODEL;
	models->model[2].type = OBJ_LIGHT_POINT;

	models->model[0].transformCount = 10;
	models->model[1].transformCount = 2;
	models->model[2].transformCount = 1;

	models->model[0].data.material.texture = 0;
	models->model[1].data.material.texture = 2;

	models->model[0].data.material.shininess = 64;
	models->model[1].data.material.shininess = 64;

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
	for (int tr = 0; tr < models->model[1].transformCount; tr++) {
		glm_vec3_copy(floorT[tr], models->model[1].translate[tr]);

		glm_vec3_copy(floorR[tr], models->model[1].rotate[tr]);
		glm_vec3_normalize(models->model[1].rotate[tr]);
	}
	glm_vec3_copy(floorS, models->model[1].scale);

	// MODEL 2
	glm_vec3_copy((vec3){1.0, 1.0, 1.0}, models->model[2].data.light.color);
	UpdateLight(&models->model[2]);

	glm_vec3_copy(lightT, models->model[2].translate[0]);

	glm_vec3_copy(lightR, models->model[2].rotate[0]);
	glm_vec3_normalize(models->model[2].rotate[0]);

	glm_vec3_copy(lightS, models->model[2].scale);

	models->model[2].data.light.attLinear = 0.027;
	models->model[2].data.light.attQuadratic = 0.0028;

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
	mouse->sensitivity = 0.005;
	mouse->firstMouse = true;

	controls->camera = camera;
	controls->mouse = mouse;
}
