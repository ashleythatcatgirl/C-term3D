
#include "shader.h"

int LoadShader(unsigned int *shaderProgram, const char *vertShader, const char *fragShader) {
	int success;
	char infoLog[512];

	const char *vertexShaderSource = GetShaderContent(vertShader);
	if (!vertexShaderSource) return -1;

	const char *fragmentShaderSource = GetShaderContent(fragShader);
	if (!fragmentShaderSource) return -1;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		free((char*)vertexShaderSource);
		free((char*)fragmentShaderSource);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

        	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	       	printf("Vertex shader fail%s\n", infoLog);
		return -1;
    	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		free((char*)vertexShaderSource);
		free((char*)fragmentShaderSource);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

        	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        	printf("Fragment shader fail%s\n", infoLog);
		return -1;
    	}

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	glLinkProgram(*shaderProgram);

	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    	if (!success) {
		free((char*)vertexShaderSource);
		free((char*)fragmentShaderSource);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

        	glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        	printf("Shader to program linking fail\n %s", infoLog);

		return -1;
    	}

	free((char*)vertexShaderSource);
	free((char*)fragmentShaderSource);

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
	if (fPtr == NULL) {
		free(content);
		return 0;
	}

	int i = 0;
	for (; (buffer = fgetc(fPtr)) != EOF; i++) {
		if (i >= size) {
			size *= 2;

			char* temp = realloc(content, sizeof(char) * size);
			if (temp == NULL) {
				free(content);
				fclose(fPtr);
				return 0;
			}

			content = temp;
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

void UpdateShaderUniform(unsigned int *shader, Models *models, Model2 *model, Camera *camera) {
	glUseProgram(*shader);

	if (model->type == OBJ_MODEL) {
		int a = 0, b = 1;
		ShaderSetInt(&model->shader, "material.texture_diffuse1", &a);
		ShaderSetInt(&model->shader, "material.texture_specular1", &b);
		ShaderSetFloat(&model->shader, "material.shininess", &model->data.material.shininess);

		ShaderSetVec3(&model->shader, "light[0].position", &models->model[12].translate);
		ShaderSetVec3(&model->shader, "light[0].ambient", &models->model[12].data.light.ambient);
		ShaderSetVec3(&model->shader, "light[0].diffuse", &models->model[12].data.light.diffuse);
		ShaderSetVec3(&model->shader, "light[0].specular", &models->model[12].data.light.specular);
		ShaderSetFloat(&model->shader, "light[0].attLinear", &models->model[12].data.light.attLinear);
		ShaderSetFloat(&model->shader, "light[0].attQuadratic", &models->model[12].data.light.attQuadratic);

		ShaderSetVec3(&model->shader, "light[1].position", &models->model[13].translate);
		ShaderSetVec3(&model->shader, "light[1].ambient", &models->model[13].data.light.ambient);
		ShaderSetVec3(&model->shader, "light[1].diffuse", &models->model[13].data.light.diffuse);
		ShaderSetVec3(&model->shader, "light[1].specular", &models->model[13].data.light.specular);
		ShaderSetFloat(&model->shader, "light[1].attLinear", &models->model[13].data.light.attLinear);
		ShaderSetFloat(&model->shader, "light[1].attQuadratic", &models->model[13].data.light.attQuadratic);

		ShaderSetVec3(&model->shader, "light[2].position", &models->model[14].translate);
		ShaderSetVec3(&model->shader, "light[2].ambient", &models->model[14].data.light.ambient);
		ShaderSetVec3(&model->shader, "light[2].diffuse", &models->model[14].data.light.diffuse);
		ShaderSetVec3(&model->shader, "light[2].specular", &models->model[14].data.light.specular);
		ShaderSetFloat(&model->shader, "light[2].attLinear", &models->model[14].data.light.attLinear);
		ShaderSetFloat(&model->shader, "light[2].attQuadratic", &models->model[14].data.light.attQuadratic);

		ShaderSetVec3(&model->shader, "camPos", &camera->position);
	} else if (model->type == OBJ_LIGHT_POINT) {
		ShaderSetVec3(&model->shader, "light.color", &model->data.light.color);
	}
}
