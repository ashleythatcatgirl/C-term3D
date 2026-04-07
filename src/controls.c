
#include "controls.h"
#include "camera.c"

void ProcessKeyInput(Window *window, Camera *camera, float deltaTime) {
	if(glfwGetKey(window->frame, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window->frame, true);
	}
	if(glfwGetKey(window->frame, GLFW_KEY_TAB) == GLFW_PRESS
	&& window->delay < glfwGetTime() - 0.25) {
		window->delay = glfwGetTime();
		glfwSetInputMode(window->frame, GLFW_CURSOR,
		glfwGetInputMode(window->frame, GLFW_CURSOR) == GLFW_CURSOR_DISABLED?
		GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	float moveSpeed = 2.5 * deltaTime;
	float turnSpeed = 2.5 * deltaTime;
	if(glfwGetKey(window->frame, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveSpeed *= 4;

	if(glfwGetKey(window->frame, GLFW_KEY_W) == GLFW_PRESS) CameraMoveZ(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_S) == GLFW_PRESS) CameraMoveZ(camera, -moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_A) == GLFW_PRESS) CameraMoveX(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_D) == GLFW_PRESS) CameraMoveX(camera, -moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_E) == GLFW_PRESS) CameraMoveY(camera, moveSpeed);
	if(glfwGetKey(window->frame, GLFW_KEY_Q) == GLFW_PRESS) CameraMoveY(camera, -moveSpeed);

	if(glfwGetKey(window->frame, GLFW_KEY_RIGHT) == GLFW_PRESS) CameraYaw(camera, turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_LEFT) == GLFW_PRESS) CameraYaw(camera, -turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_UP) == GLFW_PRESS) CameraPitch(camera, turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_DOWN) == GLFW_PRESS) CameraPitch(camera, -turnSpeed, window->frame);
	if(glfwGetKey(window->frame, GLFW_KEY_C) == GLFW_PRESS) CameraZoom(camera, 1);
	if(glfwGetKey(window->frame, GLFW_KEY_Z) == GLFW_PRESS) CameraZoom(camera, -1);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	Controls *controls = glfwGetWindowUserPointer(window);
	if (controls->mouse->firstMouse) {
		controls->mouse->lastX = xPos;
		controls->mouse->lastY = yPos;

		controls->mouse->firstMouse = false;
	}

	float xOffset = xPos - controls->mouse->lastX;
	float yOffset = controls->mouse->lastY - yPos;
	xOffset *= 0.005;
	yOffset *= 0.005;

	controls->mouse->lastX = xPos;
	controls->mouse->lastY = yPos;
	
	CameraYaw(controls->camera, xOffset, window);
	CameraPitch(controls->camera, yOffset, window);
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
	Controls *controls = glfwGetWindowUserPointer(window);

	CameraZoom(controls->camera, 2*yOffset);
}


