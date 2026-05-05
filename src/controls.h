
#ifndef CONTROLS_H
#define CONTROLS_H

#include "main.h"

void ProcessKeyInput(Window *window, Camera *camera, float deltaTime);
bool CheckKeyPress(GLFWwindow *window, int key);

void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

#endif
