
#include "main.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include "parseInput.h"

int CreateRegexPatterns(Regex *regex) {
	const char* texture = "texture [0-9][0-9]* [0-9][0-9]";
	const char* translate = "translate [0-9][0-9]* [0-9][0-9]* [xyz] [+-][0-9][0-9]*";
	const char* lightFalloff = "lightFalloff [0-9][0-9]* [0-9]\.[0-9]* [0-9]\.[0-9]*";
	const char* patterns[3] = {
		texture, translate, lightFalloff
	};

	regex->count = 3;
	regex->patterns = malloc(sizeof(regex_t) * regex->count);

	int a = 0;
	for (int r = 0; r < regex->count; r++) {
		a = regcomp(&regex->patterns[r], patterns[r], 0);
		if (a != 0) FreeRegexPatterns(regex);
	}

	return 0;
}

void FreeRegexPatterns(Regex *regex) {
	for (int r = 0; r < regex->count; r++) {
		if (regex->patterns[r].__allocated) regfree(&regex->patterns[r]);
	}
	if (regex->patterns != NULL) free(regex->patterns);
}

void SetNonBlocking() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int ParseInput(Input *input, Regex *regex, Models *models, Textures *textures) {
	input->length = read(STDIN_FILENO, input->buffer, sizeof(input->buffer)-1);

	if (input->length > 0) {
    		input->buffer[input->length] = '\0';

		CheckInput(input, regex, models, textures);
	}

	return 0;
}

int CheckInput(Input *input, Regex *regex, Models *models, Textures *textures) {
	if (!strcmp(input->buffer, "help\n")) ShowHelp();
	else if (!strcmp(input->buffer, "wireframe\n")) ToggleWireframe(input);
	else if (!regexec(&regex->patterns[0], input->buffer, 0, NULL, 0)) SetTexture(input, models, &textures->count);
	else if (!regexec(&regex->patterns[1], input->buffer, 0, NULL, 0)) SetTranslate(input, models);
	else if (!regexec(&regex->patterns[2], input->buffer, 0, NULL, 0)) SetLightFalloff(input, models);

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

void SetLightFalloff(Input *input, Models *models) {
	int light;
	float attL, attQ;
	sscanf(input->buffer, "lightFalloff %d %f %f\n", &light, &attL, &attQ);
	if (light >= models->count) return;
	Model *model = &models->model[light];
	if (model->type == OBJ_MODEL) return;
	model->data.light.attLinear = attL;
	model->data.light.attQuadratic = attQ;
}
