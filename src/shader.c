
#include "shader.h"
#include "main.h"
#include <stdbool.h>

int LoadShader(unsigned int *shaderProgram, const char *vertShader, const char *fragShader) {
	printf("Compiling shader..\n");
	const char *vertexShaderSource = GetShaderContent(vertShader);
	if (vertexShaderSource == 0) return -1;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
        	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	       	printf("Vertex shader fail%s", infoLog);
    	}

	const char *fragmentShaderSource = GetShaderContent(fragShader);
	if (fragmentShaderSource == 0) return -1;

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
        	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        	printf("Fragment shader fail%s", infoLog);
    	}

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	glLinkProgram(*shaderProgram);

	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    	if (!success) {
        	glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        	printf("Shader to program linking fail\n %s", infoLog);
    	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return 0;
}

char* GetShaderContent(const char* fileName) {
	char buffer = 0;
	char* content = 0;
	int size = 1024;

	content = (char*)malloc(sizeof(char) * size);
	if (content == NULL) return 0;

	FILE *fPtr = fopen(fileName, "r");
	if (fPtr == NULL) return 0;

	int i = 0;
	for (; (buffer = fgetc(fPtr)) != EOF; i++) {
		if (i >= size) {
			size *= 2;

			char* temp = realloc(content, sizeof(char) * size);
			if (temp == NULL) return 0;

			content = temp;
		}
		
		content[i] = buffer;
	}

	content[i] = '\0';
	
	return content;
}

void ShaderSetFloat(unsigned int *shader, const char *name, float *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform1f(location, *data);
}

void ShaderSetInt(unsigned int *shader, const char *name, int *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform1i(location, *data);
}

void ShaderSetVec3(unsigned int *shader, const char *name, vec3 *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform3fv(location, 1, *data);
}

void ShaderSetMat4(unsigned int *shader, const char *name, int gl_bool, float *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniformMatrix4fv(location, 1, gl_bool, data);
}

void UpdateShaderUniform(unsigned int *shader, Model *model, Model *light, Camera *camera) {
	glUseProgram(*shader);

	if (model->type == OBJ_MODEL) {
		ShaderSetVec3(&model->shader, "material.specular", &model->material.specular);
		ShaderSetFloat(&model->shader, "material.shininess", &model->material.shininess);

		ShaderSetVec3(&model->shader, "light.ambient", &light->light.ambient);
		ShaderSetVec3(&model->shader, "light.diffuse", &light->light.diffuse);
		ShaderSetVec3(&model->shader, "light.specular", &light->light.specular);

		ShaderSetVec3(&model->shader, "lightPos", &light->translate[0]);
		ShaderSetVec3(&model->shader, "camPos", &camera->position);
	} else if (model->type == OBJ_LIGHT) {
		ShaderSetVec3(&model->shader, "light.color", &model->light.color);
	}
}
