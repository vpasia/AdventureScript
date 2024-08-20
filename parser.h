#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"

void pushBackToken(LexItem token);
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
bool IfBlock(FILE* input, int* linenum);
bool AskBlock(FILE* input, int* linenum);
bool Choice(FILE* input, int* linenum);
bool ChoiceEffect(FILE* input, int* linenum);
bool ChoiceDialogue(FILE* input, int* linenum);
bool EntryPoint(FILE* input, int* linenum);

typedef struct
{
    char* description;
    Choice* choices;
    ConditionalStmt* conditional;
} Scene;

typedef struct
{
    char* choice;
    Effect* effect;
    Character* dialogue;
    ConditionalStmt* conditional;
} Choice;

typedef struct
{
    char* effect;
    char* scene;
    char* item;
} Effect;

typedef struct
{
    Map* dialogueScenes;
} Character;

typedef struct
{
    Effect* ifEffect;
    Effect* elseEffect;
    char* itemCondition;
    bool not;
} ConditionalStmt;

#endif