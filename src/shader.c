
#include "shader.h"
#include "include.h"
#include "main.h"
#include <stdio.h>
#include <string.h>


int LoadShader(unsigned int *shaderProgram, const char *shaderFile[3]) {
	int success;
	char infoLog[1024];

	unsigned int shader[3];
	const char *shaderSrc[3];
	const char *name[3] = {"vertex", "geometry", "fragment"};
	const char *nameC[3] = {"Vertex", "Geometry", "Fragment"};
	const int shaderType[3] = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER}; 
	bool loaded[3] = {0, 0, 0};

	for (unsigned int s = 0; s < 3; s++) {
		shaderSrc[s] = GetShaderContent(shaderFile[s]);
		if (!shaderSrc[s]) {
			printf("No %s shader found\n", name[s]);
			continue;
		}

		shader[s] = glCreateShader(shaderType[s]);
		glShaderSource(shader[s], 1, &shaderSrc[s], NULL);
		glCompileShader(shader[s]);

		glGetShaderiv(shader[s], GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader[s], 1024, NULL, infoLog);
			printf("%s shader failed to compile\nError log\n%s\nEnd\n\n", nameC[s], infoLog);

			FreeShader(shaderSrc, &shader, &loaded);
			return -1;
		}

		loaded[s] = 1;
	}

	*shaderProgram = glCreateProgram();
	for (unsigned int s = 0; s < 3; s++) {
		glAttachShader(*shaderProgram, shader[s]);
	}
	glLinkProgram(*shaderProgram);

	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    	if (!success) {
        	glGetProgramInfoLog(*shaderProgram, 1024, NULL, infoLog);
        	printf("Shader to program linking failed\nError log\n%s\nEnd\n", infoLog);

		FreeShader(shaderSrc, &shader, &loaded);
		return -1;
    	}

	FreeShader(shaderSrc, &shader, &loaded);
	
	return 0;
}

void FreeShader(const char *shaderSrc[3], unsigned int (*shader)[3], bool (*loaded)[3]) {
	for (unsigned int s = 0; s < 3; s++) {
		if (!(*loaded)[s]) continue;
		free((char*)shaderSrc[s]);
		glDeleteShader((*shader)[s]);
	}
}

char* GetShaderContent(const char* fileName) {
	char buffer = 0;
	char* content = 0;
	int size = 1024;

	content = (char*)malloc(sizeof(char) * size);
	if (content == NULL) return 0;

	FILE *fPtr = fopen(fileName, "r");
	if (fPtr == NULL) {
		free(content);
		return 0;
	}

	int i = 0;
	for (; (buffer = fgetc(fPtr)) != EOF; i++) {
		if (i >= size) {
			size *= 2;
			content = (char*)ResizeArray(content, sizeof(char) * size);
		}
		
		content[i] = buffer;
	}

	content[i] = '\0';

	fclose(fPtr);
	
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

void ShaderSetBool(unsigned int *shader, const char *name, bool *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform1i(location, *data);
}

void ShaderSetUInt(unsigned int *shader, const char *name, unsigned int *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform1ui(location, *data);
}

void ShaderSetVec3(unsigned int *shader, const char *name, vec3 *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniform3fv(location, 1, *data);
}

void ShaderSetMat4(unsigned int *shader, const char *name, int gl_bool, float *data) {	
	int location = glGetUniformLocation(*shader, name);
	glUniformMatrix4fv(location, 1, gl_bool, data);
}

void UpdateModelShader(Model *model, Scene *scene, Camera *camera) {
	glUseProgram(model->shader);

	ShaderSetUInt(&model->shader, "material.shininess", &model->shininess);
	ShaderSetBool(&model->shader, "selected", &model->selected);

	char number[8];
	char uniformNum[16], uniform[32];
	for (unsigned int l = 0; l < scene->lCount; l++) {
		sprintf(number, "%d", l);

		strcpy(uniformNum, "light[");
		strcat(uniformNum, number);

		strcpy(uniform, uniformNum);
		strcat(uniform, "].position");
		ShaderSetVec3(&model->shader, uniform, &scene->lights[l].position);
		strcpy(uniform, uniformNum);
		strcat(uniform, "].ambient");
		ShaderSetVec3(&model->shader, uniform, &scene->lights[l].ambient);
		strcpy(uniform, uniformNum);
		strcat(uniform, "].diffuse");
		ShaderSetVec3(&model->shader, uniform, &scene->lights[l].diffuse);
		strcpy(uniform, uniformNum);
		strcat(uniform, "].specular");
		ShaderSetVec3(&model->shader, uniform, &scene->lights[l].specular);
		strcpy(uniform, uniformNum);
		strcat(uniform, "].attLinear");
		ShaderSetFloat(&model->shader, uniform, &scene->lights[l].attLinear);
		strcpy(uniform, uniformNum);
		strcat(uniform, "].attQuadratic");
		ShaderSetFloat(&model->shader, uniform, &scene->lights[l].attQuadratic);
	}

	ShaderSetVec3(&model->shader, "camPos", &camera->position);
}
