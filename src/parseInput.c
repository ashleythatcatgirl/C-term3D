
#include "main.h"
#include <regex.h>
#include "parseInput.h"

void SetNonBlocking() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int ParseInput(Input *input, Models *models, Textures *textures) {
	input->length = read(STDIN_FILENO, input->buffer, sizeof(input->buffer)-1);

	if (input->length > 0) {
    		input->buffer[input->length] = '\0';

		CheckInput(input, models, textures);
	}

	return 0;
}

int CheckInput(Input *input, Models *models, Textures *textures) {
	regex_t setTexture, setTranslate;
	int regval;
	regval = regcomp(&setTexture,
			"texture [0-9][0-9]* [0-9][0-9]*", 0);
	if (regval != 0) return 1;
	regval = regcomp(&setTranslate,
			"translate [0-9][0-9]* [0-9][0-9]* [xyz] [+-][0-9][0-9]*", 0);
	if (regval != 0) return 1;

	if (!strcmp(input->buffer, "help\n")) ShowHelp();
	else if (!strcmp(input->buffer, "wireframe\n")) ToggleWireframe(input);
	else if (!regexec(&setTexture, input->buffer, 0, NULL, 0)) SetTexture(input, models, &textures->count);
	else if (!regexec(&setTranslate, input->buffer, 0, NULL, 0)) SetTranslate(input, models);

	regfree(&setTranslate);
	regfree(&setTexture);

	return 0;
}

void ShowHelp() {
	printf("== C-term3D ==\n");
	printf("\n");

	printf(" wireframe \n toggle wireframe\n\n");
	printf(" texture 'm' 't' \n change texture of model 'm' to 't'\n\n");
	printf(" translate 'm' 't' 'a' 'p' \n translate model 'm' of transform 't' on axis 'a' to position 'p'\n\n");
}

void ToggleWireframe(Input *input) {
	if (input->opts == 0b0) {
		printf("Wireframe enabled\n");
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		input->opts = 0b1;
	} else if (input->opts == 0b1) {
		printf("Wireframe disabled\n");
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		input->opts = 0b0;
	}
}

void SetTexture(Input *input, Models *models, unsigned int *textureCount) {
	int obj, tex;
	sscanf(input->buffer, "texture %d %d\n", &obj, &tex);
	if (obj >= models->count || tex >= *textureCount) return;
	if (models->model[obj].type != OBJ_MODEL) return;

	models->model[obj].data.material.texture = tex;
}

void SetTranslate(Input *input, Models *models) {
	int obj, tr;
	float pos;
	char axis;
	sscanf(input->buffer, "translate %d %d %c %f\n", &obj, &tr, &axis, &pos);
	if (obj >= models->count) return;
	if (tr >= models->model[obj].transformCount) return;
	Model *model = &models->model[obj];
	switch (axis) {
	case 'x':
		model->translate[tr][0] = pos;
		break;
	case 'y':
		model->translate[tr][1] = pos;
		break;
	case 'z':
		model->translate[tr][2] = pos;
		break;
	}
}
