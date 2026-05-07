
#ifndef SHADER_H
#define SHADER_H

#include "main.h"

char* GetShaderContent(const char* fileName);
int LoadShader(unsigned int *shaderProgram, const char *shaderFile[3]);
void FreeShader(const char *vShaderSrc[3], unsigned int (*shader)[3], bool (*loaded)[3]);

void ShaderSetFloat(unsigned int *shader, const char *name, float *data);
void ShaderSetInt(unsigned int *shader, const char *name, int *data);
void ShaderSetUInt(unsigned int *shader, const char *name, unsigned int *data);
void ShaderSetBool(unsigned int *shader, const char *name, bool *data);
void ShaderSetVec3(unsigned int *shader, const char *name, vec3 *data);
void ShaderSetMat4(unsigned int *shader, const char *name, int gl_bool, float *data);

void UpdateModelShader(Model *model, Scene *scene, Camera *camera);

#endif
