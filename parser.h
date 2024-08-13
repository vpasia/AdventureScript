#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"

LexItem pushbackToken(LexItem token);
LexItem getNextProgToken(FILE* input, int* linenum);

bool Prog(FILE* input, int* linenum);
bool Decl(FILE* input, int* linenum);
bool ItemDecl(FILE* input, int* linenum);
bool CharacterDecl(FILE* input, int* linenum);
bool DialogueContent(FILE* input, int* linenum);
bool DialogueChoice(FILE* input, int* linenum);
bool DialogueEffect(FILE* input, int* linenum);
bool SceneDefinition(FILE* input, int* linenum);
bool SceneContent(FILE* input, int* linenum);
bool Description(FILE* input, int* linenum);
bool AskBlock(FILE* input, int* linenum);
bool Choice(FILE* input, int* linenum);
bool ChoiceEffect(FILE* input, int* linenum);
bool EntryPoint(FILE* input, int* linenum);

#endif