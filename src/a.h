
#include "include/cglm/cglm.h"

typedef enum {
	OBJ_MESH
} ModelType;
typedef enum {
	LIGHT_POINT,
	LIGHT_DIRECTION,
	LIGHT_SPOT,
	LIGHT_AREA
} LightType;

typedef struct Transforms {
	vec3 translation;
	vec3 rotation;
	vec3 scaling;
} Transforms;

typedef struct Model {
	Transforms transforms;
	
	unsigned int VBO;
	unsigned int VAO;

	unsigned int shader;

	ModelType type;
} Model;
typedef struct Light {
	Transforms transforms;

	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float attLinear;
	float attQuadratic;
	
	LightType type;
} Light;

typedef struct Objects {
	Model *models;
	Light *lights;
} Objects;

typedef struct Camera {
	
	vec3 position;
	vec3 target;
	vec3 front;
	vec3 right;

	vec3 direction;

	float pitch;
	float yaw;

	float zoom;

	float moveSpeed;
	float turnSpeed;
} Camera;

