
#ifndef CAMERA_H 
#define CAMERA_H

#include "main.h"

void CameraMoveZ(Camera *camera, float moveSpeed);
void CameraMoveX(Camera *camera, float moveSpeed);
void CameraMoveY(Camera *camera, float moveSpeed);

void CameraYaw(Camera *camera, float turnSpeed);
void CameraPitch(Camera *camera, float turnSpeed);

void CameraZoom(Camera *camera, float zoomSpeed);

#endif
