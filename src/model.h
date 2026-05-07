
#ifndef MODEL_H 
#define MODEL_H

#include "include.h"

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

	vec3 position, rotation, scale;
	float rotationAngle;

	unsigned int mCount, mSize;
	unsigned int tCount, tSize;
	unsigned int shininess;
	unsigned int shader;

	char directory[64];

	bool selected;
} Model;

typedef struct Light {
	vec3 position, rotation, scale;

	vec3 color;
	vec3 ambient, diffuse, specular;
	float attLinear, attQuadratic;

	unsigned int shader;
} Light;

typedef struct Scene {
	Model *models;
	Light *lights;

	int mSelected;
	
	unsigned int mCount, lCount;
	unsigned int debugPointVAO, debugPointVBO;
	unsigned int floorShader;
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

