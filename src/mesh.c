
#include "mesh.h"
#include "main.h"
#include "shader.c"
#include <stdio.h>
#include <string.h>

void DrawMesh(Mesh *mesh, unsigned int *shader) {
	unsigned int diffuseN = 1, specularN = 1;

	char number[8], name[32];
	char floatName[64];
	for (unsigned int t = 0; t < mesh->tCount; t++) {
		glActiveTexture(GL_TEXTURE0 + t);

		strcpy(name, mesh->textures[t].type);

		if (!strcmp(name, "texture_diffuse")) {
			sprintf(number, "%d", diffuseN);
		} else if (!strcmp(name, "texture_specular")) {
			sprintf(number, "%d", specularN);
		}

		strcpy(floatName, "material.");
		strcat(floatName, name);
		strcat(floatName, number);

		ShaderSetFloat(shader, floatName, (float*)&t);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[t].id);
	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->iCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
 
void DrawModel(Model *model) {
	for (unsigned int m = 0; m < model->mCount; m++) {
		DrawMesh(&model->meshes[m], &model->shader);
	}
}

void SetupMesh(Mesh *mesh) {
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);
	glGenBuffers(1, &mesh->EBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->iCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCords));
	glEnableVertexAttribArray(2);
}

void LoadModel(Model *model, const char *file) {
	const struct aiScene *scene = aiImportFile(file,
		aiProcess_Triangulate |
		aiProcess_FlipUVs
	);

	model->mCount = 0;
	model->mSize = scene->mNumMeshes;
	model->meshes = malloc(sizeof(Mesh) * model->mSize);
	if (!model->meshes) return;

	model->tCount = 0;
	model->tSize = 1;
	model->savedTextures = malloc(sizeof(Texture) * model->tSize);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("%s", aiGetErrorString());
		return;
	}

	unsigned int pos = 0;
	for (unsigned int c = 0; file[c] != '\0'; c++) {
		if (file[c] == '/') pos = c;
	}
	if (pos == 0) return;
	strncpy(model->directory, file, pos);
	model->directory[pos] = '\0';
	ProcessNode(model, scene->mRootNode, scene);

	aiReleaseImport(scene);
	return;
}

int ProcessNode(Model *model, struct aiNode *aiNode, const struct aiScene *aiScene) {
	struct aiMesh *aiMesh = NULL;
	Mesh *mesh = NULL;
	for (unsigned int m = 0; m < aiNode->mNumMeshes; m++) {
		aiMesh = aiScene->mMeshes[aiNode->mMeshes[m]];

		mesh = ProcessMesh(model, aiMesh, aiScene);
		model->meshes[model->mCount] = *mesh;
		SetupMesh(&model->meshes[model->mCount++]);
	}

	for (unsigned int n = 0; n < aiNode->mNumChildren; n++) {
		ProcessNode(model, aiNode->mChildren[n], aiScene);
	}

	return 0;
}

Mesh *ProcessMesh(Model *model, struct aiMesh *aiMesh, const struct aiScene *aiScene) {
	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->vCount = aiMesh->mNumVertices;
	mesh->vertices = malloc(sizeof(Vertex) * mesh->vCount);

	mesh->tCount = 0;
	mesh->textures = NULL;

	mesh->iCount = aiMesh->mNumFaces * 3;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->iCount);

	for (unsigned int v = 0; v < mesh->vCount; v++) {
		glm_vec3_copy((vec3){
			aiMesh->mVertices[v].x,
			aiMesh->mVertices[v].y,
			aiMesh->mVertices[v].z
		}, mesh->vertices[v].position);

		glm_vec3_copy((vec3){
			aiMesh->mNormals[v].x,
			aiMesh->mNormals[v].y,
			aiMesh->mNormals[v].z
		}, mesh->vertices[v].normal);

		if (!aiMesh->mTextureCoords[0]) {
			glm_vec2_copy((vec2){0.0, 0.0}, mesh->vertices[v].textureCords);
			continue;
		}

		glm_vec2_copy((vec2){
			aiMesh->mTextureCoords[0][v].x,
			aiMesh->mTextureCoords[0][v].y
		}, mesh->vertices[v].textureCords);
	}


	unsigned int iCount = 0;
	for (unsigned int f = 0; f < aiMesh->mNumFaces; f++) {
		for (unsigned int i = 0; i < 3; i++) {
			mesh->indices[iCount++] = aiMesh->mFaces[f].mIndices[i];
		}
	}

	if (aiMesh->mMaterialIndex < 0) return mesh;

	struct aiMaterial *aiMaterial = aiScene->mMaterials[aiMesh->mMaterialIndex];

	unsigned int diffuseCount = aiGetMaterialTextureCount(aiMaterial, aiTextureType_DIFFUSE);
	unsigned int specularCount = aiGetMaterialTextureCount(aiMaterial, aiTextureType_SPECULAR);

	Texture *diffuseMaps = LoadMaterialTextures(model, aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse", diffuseCount);
	Texture *specularMaps = LoadMaterialTextures(model, aiMaterial, aiTextureType_SPECULAR, "texture_specular", specularCount);

	mesh->textures = (Texture*)ResizeArray(
		mesh->textures,
		sizeof(Texture) * (mesh->tCount + diffuseCount + specularCount)
	);

	for (unsigned int i = 0; i < diffuseCount; i++) {
		mesh->textures[i + mesh->tCount] = diffuseMaps[i];
	}
	mesh->tCount += diffuseCount;
	for (unsigned int i = 0; i < specularCount; i++) {
		mesh->textures[i + mesh->tCount] = specularMaps[i];
	}
	mesh->tCount += specularCount;

	return mesh;
}

Texture *LoadMaterialTextures(Model *model, struct aiMaterial *aiMaterial, enum aiTextureType type, const char *typeName, unsigned int textureCount) {
	Texture *textures = malloc(sizeof(Texture) * textureCount);

	struct aiString aiString;
	bool skip = false;
	for (unsigned int t = 0; t < textureCount; t++) {
		skip = false;
		aiGetMaterialTexture(aiMaterial, type, t, &aiString, 0, 0, 0, 0, 0, 0);
		for (unsigned int j = 0; j < model->tCount; j++) {
			if (strcmp(model->savedTextures[j].name, aiString.data) != 0) continue;
			
			skip = true;
			textures[t].id = model->savedTextures[j].id;
			strcpy(textures[t].name, model->savedTextures[j].name);
			strcpy(textures[t].type, model->savedTextures[j].type);
			break;
		}

		if (skip) continue;

		textures[t].id = LoadTextureFile(model, aiString.data);
		strcpy(textures[t].type, typeName);
		strcpy(textures[t].name, aiString.data);

		if (model->tCount >= model->tSize) {
			if (model->tSize == 0) model->tSize = 1;
			model->tSize *= 2;
			model->savedTextures = (Texture*)ResizeArray(
				model->savedTextures,
				model->tSize * sizeof(Texture)
			);
		}

		model->savedTextures[model->tCount].id = textures[t].id;
		strcpy(model->savedTextures[model->tCount].name, textures[t].name);
		strcpy(model->savedTextures[model->tCount].type, textures[t].type);
		model->tCount++;
	}
	
	return textures;
}

unsigned int LoadTextureFile(Model *model, const char *fileName) {
	char path[64];
	strcpy(path, model->directory);
	strcat(path, "/");
	strcat(path, fileName);

	unsigned int textureId;
	unsigned char *data;
	int width, height, channels;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load(path, &width, &height, &channels, 4);
	if (!data) return -1;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return textureId;
}
