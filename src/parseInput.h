
#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include "main.h"

void SetNonBlocking();
int CreateRegexPatterns(Regex *regex);
int NewPattern(regex_t preg, const char *pattern);
void FreeRegexPatterns(Regex *regex);

int ParseInput(Input *input, Regex *regex, Models *models, Textures *textures);
int CheckInput(Input *input, Regex *regex, Models *models, Textures *textures);

void ShowHelp();
void ToggleWireframe(Input *input);
void SetTexture(Input *input, Models *models, unsigned int *textureCount);
void SetTranslate(Input *input, Models *models);
void SetLightFalloff(Input *input, Models *models);

#endif
