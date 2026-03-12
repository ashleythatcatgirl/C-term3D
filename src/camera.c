
#include "camera.h"
#include "include/cglm/vec3.h"
#include "main.h"

void CameraMoveZ(Camera *camera, float moveSpeed) {
	vec3 move;
	glm_vec3_scale(camera->front, moveSpeed, move);
	glm_vec3_add(camera->position, move, camera->position);	
}
void CameraMoveX(Camera *camera, float moveSpeed) {
	vec3 move;
	glm_cross(camera->front, (vec3){0.0, 1.0, 0.0}, camera->right);
	glm_normalize(camera->right);
	glm_vec3_scale(camera->right, moveSpeed, move);
	glm_vec3_sub(camera->position, move, camera->position);	
}
void CameraMoveY(Camera *camera, float moveSpeed) {
	vec3 move;
	glm_vec3_scale((vec3){0.0, 1.0, 0.0}, moveSpeed, move);
	glm_vec3_add(camera->position, move, camera->position);	
}

void CameraYaw(Camera *camera, float turnSpeed) {
	camera->yaw += turnSpeed;

	glm_vec3_copy((vec3){
		cos(camera->pitch) * cos(camera->yaw),
		sin(camera->pitch),
		cos(camera->pitch) * sin(camera->yaw)
	}, camera->front);
	glm_normalize(camera->front);
}
void CameraPitch(Camera *camera, float turnSpeed) {
	if (camera->pitch + turnSpeed <= -1.57
	|| camera->pitch + turnSpeed >= 1.57) return;

	camera->pitch += turnSpeed;

	camera->front[0] = cos(camera->pitch) * cos(camera->yaw);
	camera->front[1] = sin(camera->pitch);
	camera->front[2] = cos(camera->pitch) * sin(camera->yaw);
	glm_normalize(camera->front);
}

void CameraZoom(Camera *camera, float zoomSpeed) {
	if (camera->zoom - zoomSpeed < 1
	|| camera->zoom - zoomSpeed >= 90) return;

	camera->zoom -= zoomSpeed;
}
