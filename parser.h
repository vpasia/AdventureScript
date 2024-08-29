#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "lexer.h"
#include "map.h"
#include "linkedlist.h"

extern Map* scenes;
extern Map* playerInventory;
extern Map* items;
extern Map* characters;

void pushBackToken(LexItem* token);
LexItem getNextProgToken(FILE* input, int* linenum);
void FreeUtilMaps();

bool Prog(FILE* input, int* linenum);
bool Decl(FILE* input, int* linenum);
bool ItemDecl(FILE* input, int* linenum);
bool CharacterDecl(FILE* input, int* linenum);
bool DialogueContent(FILE* input, int* linenum, Character* character);
bool SceneDefinition(FILE* input, int* linenum);
bool SceneContent(FILE* input, int* linenum); 
bool Description(FILE* input, int* linenum);
bool IfBlock(FILE* input, int* linenum, Scene* scene, Choice* choice);
bool AskBlock(FILE* input, int* linenum, Scene* scene);
bool ChoiceDefinition(FILE* input, int* linenum, Scene* scene);
bool EffectDefinition(FILE* input, int* linenum, Scene* scene, Choice* choice);
bool EntryPoint(FILE* input, int* linenum);

typedef struct
{
    char* description;
    char* prompt;
    LinkedList* choices;
    ConditionalStmt* conditional;
} Scene;

typedef struct
{
    char* choiceDescription;
    Effect* effect;
    ConditionalStmt* conditional;
} Choice;

typedef enum {SC, IT, CH} EffectType;

typedef union 
{
    char* scene;
    char* item;
    char* character;
} EffectAction;

typedef struct
{
    char* effectDescription;
    EffectType type;
    EffectAction action;
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