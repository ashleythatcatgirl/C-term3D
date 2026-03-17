
#include "main.h"
#include <stdio.h>
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
	regex_t r1;
	int regval;
	regval = regcomp(&r1, "model [0-9][0-9]* texture [0-9][0-9]*", 0);
	if (regval != 0) printf("fuck\n");

	if (!strcmp(input->buffer, "help\n")) ShowHelp();
	else if (!strcmp(input->buffer, "wireframe\n")) ToggleWireframe(input);
	else if (!regexec(&r1, input->buffer, 0, NULL, 0)) SetTexture(input, models, textures);

	return 0;
}

void ShowHelp() {
	printf("== C-term3D ==\n");
	printf("\n");

	printf(" wireframe \t\t toggle wireframe\n");
	printf(" model 'm' texture 't' \t change texture of model 'm' to 't'\n");

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

void SetTexture(Input *input, Models *models, Textures *textures) {
	int obj, tex;
	sscanf(input->buffer, "model %d texture %d\n", &obj, &tex);
	if (obj >= models->count || tex > textures->count) return;
	models->model[obj].texture = textures->texture[tex].memory;
}
