
#include "main.h"
#ifndef SHADER_H
#define SHADER_H

char* GetShaderContent(const char* fileName);
int LoadShader(unsigned int *shaderProgram, const char *vertShader, const char *fragShader);

void ShaderSetVec3(unsigned int *shader, const char *name, vec3 *data);
void ShaderSetMat4(unsigned int *shader, const char *name, int gl_bool, float *data);
void ShaderSetFloat(unsigned int *shader, const char *name, float *data);
void ShaderSetInt(unsigned int *shader, const char *name, int *data);

#endif
