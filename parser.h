#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"

void pushBackToken(LexItem* token);
LexItem getNextProgToken(FILE* input, int* linenum);

bool Prog(FILE* input, int* linenum);
bool Decl(FILE* input, int* linenum);
bool ItemDecl(FILE* input, int* linenum);
bool CharacterDecl(FILE* input, int* linenum);
bool DialogueContent(FILE* input, int* linenum, Character* character);
bool SceneDefinition(FILE* input, int* linenum);
bool SceneContent(FILE* input, int* linenum); 
bool Description(FILE* input, int* linenum);
bool IfBlock(FILE* input, int* linenum, Scene* scene);
bool AskBlock(FILE* input, int* linenum, Scene* scene);
bool ChoiceDefinition(FILE* input, int* linenum, Scene* scene);
bool EffectDefinition(FILE* input, int* linenum, Scene* scene);
bool EntryPoint(FILE* input, int* linenum);

typedef struct
{
    char* description;
    char* prompt;
    Choice* choices;
    ConditionalStmt* conditional;
} Scene;

typedef struct
{
    char* choice;
    Effect* effect;
    ConditionalStmt* conditional;
} Choice;

typedef enum {SC, IT, CH} EffectType;

typedef struct
{
    char* effect;
    EffectType type;
    char* scene;
    char* item;
    char* character;
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