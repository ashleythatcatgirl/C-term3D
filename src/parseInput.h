
#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include "main.h"
#include "model.h"

void SetNonBlocking();
int CreateRegexPatterns(Regex *regex);
int NewPattern(regex_t preg, const char *pattern);
void FreeRegexPatterns(Regex *regex);

int ParseInput(Input *input, Regex *regex, Scene *scene);
int CheckInput(Input *input, Regex *regex, Scene *scene);

void ShowHelp();
void ToggleWireframe(Input *input);
void ToggleFps(Input *input);
void ToggleVsync(Input *input);

void Select(Input *input, Scene *scene);

void SetPosition(Input *input, Scene *scene);
void SetRotation(Input *input, Scene *scene);
void SetScale(Input *input, Scene *scene);

#endif
