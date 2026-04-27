
#ifndef MESH_H
#define MESH_H

#include "main.h"

typedef struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 textureCords;
} Vertex;

typedef struct Texture {
	unsigned int id;
	char type[64];
	char name[64];
} Texture;

typedef struct Mesh {
	Vertex *vertices;
	Texture *textures;
	unsigned int *indices;

	unsigned int vCount, tCount, iCount;

	unsigned int VAO, VBO, EBO;
} Mesh;

typedef struct Model {
	Mesh *meshes;
	Texture *savedTextures;

	unsigned int mCount, mSize;
	unsigned int tCount, tSize;
	unsigned int shader;

	char directory[64];
} Model;

typedef struct Scene {
	Model *models;

	unsigned int mCount;
} Scene;

void SetupMesh(Mesh *mesh);
void DrawMesh(Mesh *mesh, unsigned int *shader);

void LoadModel(Model *model, const char *pFile);
void DrawModel(Model *model);

int ProcessNode(Model *model, struct aiNode *node, const struct aiScene *scene);
Mesh *ProcessMesh(Model *model, struct aiMesh *mesh, const struct aiScene *scene);
Texture *LoadMaterialTextures(Model *model, struct aiMaterial *aiMaterial, enum aiTextureType type, const char *typeName, unsigned int textureCount);
unsigned int LoadTextureFile(Model *model, const char *fileName);

#endif
