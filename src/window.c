
#include "window.h"
#include "controls.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
} 

int CreateWindow(Window *window, Controls *controls) {
	printf("Window initializiation..\n");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window->frame = glfwCreateWindow(window->width, window->height, "C-term3D", NULL, NULL);
	if (window->frame == NULL) {
		printf("Window creation error\n");
		return 1;
	}

	glfwSetWindowUserPointer(window->frame, (void*)controls);

	glfwMakeContextCurrent(window->frame);
	glfwSetFramebufferSizeCallback(window->frame, framebuffer_size_callback);  
	glfwSetCursorPosCallback(window->frame, mouse_callback);  
	glfwSetScrollCallback(window->frame, scroll_callback);

	glfwSetInputMode(window->frame, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("GLAD error\n");
		return 1;
	}

	return 0;
}

