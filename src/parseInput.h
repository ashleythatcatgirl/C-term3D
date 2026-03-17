
#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include "main.h"

void SetNonBlocking();
int ParseInput(Input *input, Models *models, Textures *textures);
int CheckInput(Input *input, Models *models, Textures *textures);

void ShowHelp();
void ToggleWireframe(Input *input);
void SetTexture(Input *input, Models *models, Textures *textures);

#endif
