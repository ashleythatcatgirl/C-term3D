
#include "parseInput.h"
#include "main.h"
#include "model.h"
#include <complex.h>
#include <stdbool.h>

const char *infoMsg = "\033[34;1;4mInfo:\033[0m ";
const char *warningMsg = "\033[33;1;4mWarning:\033[0m ";
const char *errorMsg = "\033[31;1;4mError:\033[0m ";

int CreateRegexPatterns(Regex *regex) {
	const char *select = "select [0-9][0-9]*";
	const char *setPosition = "set position -\\?[0-9]*\\.\\?[0-9]* -\\?[0-9]*\\.\\?[0-9]* -\\?[0-9]*\\.\\?[0-9]*";
	const char *setRotation = "set rotation -\\?[0-9]*\\.\\?[0-9]* -\\?[0-9]*\\.\\?[0-9]* -\\?[0-9]*\\.\\?[0-9]*";
	const char *setScale = "set scale [0-9]*\\.\\?[0-9]* [0-9]*\\.\\?[0-9]* [0-9]*\\.\\?[0-9]*";
	const char *patterns[4] = {
		select, setPosition, setRotation, setScale 
	};

	regex->count = 4;
	regex->patterns = malloc(sizeof(regex_t) * regex->count);

	int a = 0;
	for (int r = 0; r < regex->count; r++) {
		a = regcomp(&regex->patterns[r], patterns[r], 0);
		if (a != 0) FreeRegexPatterns(regex);
	}

	return 0;
}

void FreeRegexPatterns(Regex *regex) {
	if (regex->patterns != NULL) return;
	for (int r = 0; r < regex->count; r++) {
		regfree(&regex->patterns[r]);
	}
	free(regex->patterns);
}

void SetNonBlocking() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int ParseInput(Input *input, Regex *regex, Scene *scene) {
	input->length = read(STDIN_FILENO, input->buffer, sizeof(input->buffer)-1);

	if (input->length > 0) {
    		input->buffer[input->length] = '\0';

		CheckInput(input, regex, scene);
	}

	return 0;
}

int CheckInput(Input *input, Regex *regex, Scene *scene) {
	if (!strcmp(input->buffer, "help\n")) ShowHelp();
	else if (!strcmp(input->buffer, "toggle wireframe\n")) ToggleWireframe(input);
	else if (!strcmp(input->buffer, "toggle fps\n")) ToggleFps(input);
	else if (!strcmp(input->buffer, "toggle vsync\n")) ToggleVsync(input);
	else if (!regexec(&regex->patterns[0], input->buffer, 0, NULL, 0)) Select(input, scene);
	else if (!regexec(&regex->patterns[1], input->buffer, 0, NULL, 0)) SetPosition(input, scene);
	else if (!regexec(&regex->patterns[2], input->buffer, 0, NULL, 0)) SetRotation(input, scene);
	else if (!regexec(&regex->patterns[3], input->buffer, 0, NULL, 0)) SetScale(input, scene);
	else printf("%sNo entry for command:\n%sList available commmands with \'help\';\n\n", errorMsg, input->buffer);

	return 0;
}

void ShowHelp() {
	printf("== C-term3D ==\n\n");

	printf(" toggle wireframe\n toggle wireframe mode\n\n");
	printf(" toggle fps\n toggle fps display\n\n");
	printf(" toggle vsync\n toggle vsync (limits the fps of the program to your screens refresh rate, usually 60 fps)\n\n");
	printf(" select 'm'\n select/deselect model 'm'\n\n");
	printf(" set position 'x' 'y' 'z'\n set position of selected model to 'x''y''z'\n\n");
	printf(" set rotation 'a' 'b' 'c'\n set rotation of selected model to 'a''b''c'\n\n");
	printf(" set scale 'x' 'y' 'z'\n set scale of selected model to 'x''y''z'\n\n");
}

void ToggleWireframe(Input *input) {
	if (input->wireframe == false) {
		printf("%sWireframe enabled;\n", infoMsg);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		input->wireframe = true;
	} else if (input->wireframe == true) {
		printf("%sWireframe disabled;\n", infoMsg);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		input->wireframe = false;
	}
}
void ToggleFps(Input *input) {
	if (input->fps == false) {
		printf("%sFps enabled;\n", infoMsg);
		input->fps = true;
	} else if (input->fps == true) {
		printf("%sFps disabled;\n", infoMsg);
		input->fps = false;
	}
}
void ToggleVsync(Input *input) {
	if (input->vsync == false) {
		printf("%sVsync enabled;\n", infoMsg);
		glfwSwapInterval(1);
		input->vsync = true;
	} else if (input->vsync == true) {
		printf("%sVsync disabled;\n", infoMsg);
		glfwSwapInterval(0);
		input->vsync = false;
	}
}

void Select(Input *input, Scene *scene) {
	Model *newModel = NULL;
	unsigned int obj;
	sscanf(input->buffer, "select %d", &obj); 
	if (obj < 0 || obj >= scene->mCount) {
		printf("%sModel doesnt exist;\n\n", errorMsg);
		return;
	}
	newModel = &scene->models[obj];

	if (scene->mSelected == obj) {
		printf("%sDeselected model %d;\n\n", infoMsg, scene->mSelected);
		scene->models[scene->mSelected].selected = false;
		scene->mSelected = -1;
		return;
	} else if (scene->mSelected >= 0) {
		printf("%sDeselected model %d;\n\n", infoMsg, scene->mSelected);
		scene->models[scene->mSelected].selected = false;
	}

	printf("%sSelected model %d;\n", infoMsg, obj);
	printf("Position: %f, %f, %f;\n", newModel->position[0], newModel->position[1], newModel->position[2]);
	printf("Rotation: %f, %f, %f;\n", newModel->rotation[0], newModel->rotation[1], newModel->rotation[2]);
	printf("Scale: %f, %f, %f;\n\n", newModel->scale[0], newModel->scale[1], newModel->scale[2]);
	newModel->selected = true;
	scene->mSelected = obj;
}

void SetPosition(Input *input, Scene *scene) {
	vec3 position;
	sscanf(input->buffer, "set position %f %f %f", &position[0], &position[1], &position[2]); 

	if (scene->mSelected < 0) {
		printf("%sNo models selected;\n\n", errorMsg);
		return;
	}

	printf("%sSet position of model %d to (%f,%f,%f);\n\n", infoMsg, scene->mSelected,
		position[0], position[1], position[2]);
	glm_vec3_copy(position, scene->models[scene->mSelected].position);
}
void SetRotation(Input *input, Scene *scene) {
	vec3 rotation;
	sscanf(input->buffer, "set rotation %f %f %f", &rotation[0], &rotation[1], &rotation[2]); 

	if (scene->mSelected < 0) {
		printf("%sNo models selected;\n\n", errorMsg);
		return;
	}

	printf("%sSet rotation of model %d to (%f,%f,%f);\n\n", infoMsg, scene->mSelected,
		rotation[0], rotation[1], rotation[2]);
	glm_vec3_copy(rotation, scene->models[scene->mSelected].rotation);
	glm_vec3_normalize(scene->models[scene->mSelected].rotation);
}
void SetScale(Input *input, Scene *scene) {
	vec3 scale;
	sscanf(input->buffer, "set scale %f %f %f", &scale[0], &scale[1], &scale[2]); 

	if (scene->mSelected < 0) {
		printf("%sNo models selected;\n\n", errorMsg);
		return;
	}

	printf("%sSet scale of model %d to (%f,%f,%f);\n\n", infoMsg, scene->mSelected,
		scale[0], scale[1], scale[2]);
	glm_vec3_copy(scale, scene->models[scene->mSelected].scale);
}
