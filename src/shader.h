
#ifndef SHADER_H
#define SHADER_H

#include "main.h"

char* GetShaderContent(const char* fileName);
int LoadShader(unsigned int *shaderProgram, const char *vertShader, const char *fragShader);

void ShaderSetFloat(unsigned int *shader, const char *name, float *data);
void ShaderSetInt(unsigned int *shader, const char *name, int *data);
void ShaderSetUInt(unsigned int *shader, const char *name, unsigned int *data);
void ShaderSetVec3(unsigned int *shader, const char *name, vec3 *data);
void ShaderSetMat4(unsigned int *shader, const char *name, int gl_bool, float *data);

void UpdateShaderUniform(unsigned int *shader, Models *models, Model *model, Camera *camera);

#endif
