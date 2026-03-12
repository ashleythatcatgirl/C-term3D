
#include "main.h"
#include "parseInput.h"

void SetNonBlocking() {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int ParseInput(Input *input) {
	input->length = read(STDIN_FILENO, input->buffer, sizeof(input->buffer)-1);

	if (input->length > 0) {
    		input->buffer[input->length] = '\0';

    		for (int i = 0; i < input->length; i++) {
       			if (input->buffer[i] == '\n') {
        		}
    		}

		printf("Input: %s", input->buffer);
	}

	return 0;
}
