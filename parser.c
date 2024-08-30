#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

#define ParseError(message, linenum) printf("%d: %s \n", *linenum, message);

Map* scenes = NULL;
Map* playerInventory = NULL;
Map* items = NULL;
Map* characters = NULL;

static pushedBack = false;
static LexItem pushedBackToken;

LexItem getNextProgToken(FILE* input, int* linenum)
{
    if(pushedBack)
    {
        pushedBack = false;
        return pushedBackToken;
    }
    return getNextToken(input, linenum);
}

void pushBackToken(LexItem* token)
{
    if(pushedBack)
    {
        exit(1);
    }
    pushedBack = true;
    pushedBackToken = *token;
}

void FreeUtilMaps()
{
    freeMap(scenes);
    freeMap(playerInventory);
    freeMap(items);
    freeMap(characters);
}

bool InitializeUtilMaps()
{
    scenes = createMap();
    playerInventory = createMap();
    items = createMap();
    characters = createMap();

    return scenes && playerInventory && items && characters;
}

bool Prog(FILE* input, int* linenum)
{
    if(!InitializeUtilMaps())
    {
        ParseError("Failed to Initialize Game Data Containers.", linenum);
        return false;
    }

    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token == ERR)
    {
        char buffer[27 + strlen(tok.lexeme)];
        sprintf(&buffer, "Unrecognized Input Pattern %s", tok.lexeme);
        free(tok.lexeme);

        ParseError(buffer, linenum);
        FreeUtilMaps();
        return false;
    }
    else if(tok.token != ITEM || tok.token != CHARACTER || tok.token != SCENE)
    {
        ParseError("Must Define Elements of Story First!", linenum);
        FreeUtilMaps();
        return false;
    }

    pushBackToken(&tok);

    if(!Decl(input, linenum))
    {
        ParseError("Incorrect Declaration in Program", linenum);
        FreeUtilMaps();
        return false;
    }

    if(!SceneDefinition(input, linenum))
    {
        ParseError("Incorrect Scene Definition in Program", linenum);
        FreeUtilMaps();
        return false;
    }

    tok = getNextProgToken(input, linenum);

    if(tok.token != START)
    {
        ParseError("Missing Entry Point in Program", linenum);
        FreeUtilMaps();
        return false;
    }

    free(tok.lexeme);
    tok = getNextProgToken(input, linenum);

    /* TODO: Segway into game simulator */

    return true;
}

bool Decl(FILE* input, int* linenum)
{
    bool status;
    LexItem tok = getNextProgToken(input, linenum);

    switch(tok.token)
    {
        case ITEM:
            status = ItemDecl(input, linenum);
            free(tok.lexeme);

            if(status) status = Decl(input, linenum);
            break;
        
        case CHARACTER:
            status = CharacterDecl(input, linenum);
            free(tok.lexeme);

            if(status) status = Decl(input, linenum);
            break;
        
        default:
            pushBackToken(&tok);
            return true;
    }

    return status;
}

bool ItemDecl(FILE* input, int* linenum)
{
    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        free(tok.lexeme);
        ParseError("Missing Item name in Declaration.", linenum);
        return false;
    }

    if(!setItem(items, tok.lexeme, true))
    {
        free(tok.lexeme);
        ParseError("Failed to add Item in declared items.", linenum);
        return false;
    }

    return true;
}

bool CharacterDecl(FILE* input, int* linenum)
{
    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        free(tok.lexeme);
        ParseError("Missing Character Name in Definition.", linenum);
        return false;
    }

    char* characterName = tok.lexeme;
    Character* character = malloc(sizeof(Character));

    if(!character)
    {
        free(characterName);
        ParseError("Unable to allocate memory for character.", linenum);
        return false;
    }

    character->dialogueScenes = createMap();

    if(!character->dialogueScenes)
    {
        free(characterName);
        freeMap(character->dialogueScenes);
        free(character);

        ParseError("Unable to allocate memory for character dialogue scenes.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != LCURLY)
    {
        free(characterName);
        freeMap(character->dialogueScenes);
        free(character);
        
        ParseError("Missing { in Character Definition.", linenum);
        return false;
    }
    
    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != DIALOGUE)
    {
        free(characterName);
        freeMap(character->dialogueScenes);
        free(character);

        ParseError("Must at least define one dialogue scene.", linenum);
        return false;
    }

    while(tok.token == DIALOGUE)
    {
        if(!DialogueContent(input, linenum, character))
        {
            free(characterName);
            freeMap(character->dialogueScenes);
            free(character);

            return false;
        }

        tok = getNextProgToken(input, linenum);
        free(tok.lexeme);
    }

    if(tok.token != RCURLY)
    {
        free(characterName);
        freeMap(character->dialogueScenes);
        free(character);

        ParseError("Missing } in Character Definition.", linenum);
        return false;
    }

    if(!setItem(characters, characterName, character))
    {
        free(characterName);
        freeMap(character->dialogueScenes);
        free(character);

        ParseError("Failed to add Character in defined characters.", linenum);
        return false;
    }

    return true;
}

bool DialogueContent(FILE* input, int* linenum, Character* character)
{
    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token != STRING) 
    {
        free(tok.lexeme);
        ParseError("Missing Dialogue name in Dialogue definition.", linenum);
        return false;
    }

    char* dialogueName = tok.lexeme;
    Scene* dialogueScene = malloc(sizeof(Scene));

    if(!dialogueScene)
    {
        free(dialogueName);
        free(dialogueScene);

        ParseError("Failed to allocate memory for dialogue scene.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != LCURLY)
    {
        free(dialogueName);
        free(dialogueScene);

        ParseError("Missing { in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != SAY) 
    {
        free(dialogueName);
        free(dialogueScene);

        ParseError("Missing SAY keyword in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        free(dialogueName);
        free(dialogueScene);

        ParseError("Missing Character Dialogue in Dialogue definition.", linenum);
        free(tok.lexeme);
        return false;
    }

    dialogueScene->description = tok.lexeme;

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != ASK)
    {
        free(dialogueName);
        free(dialogueScene->description);
        free(dialogueScene);

        ParseError("Missing ASK block in Dialogue definition.", linenum);
        return false;
    }

    if(!AskBlock(input, linenum, dialogueScene))
    {
        free(dialogueName);
        free(dialogueScene->description);
        free(dialogueScene);

        ParseError("Incorrect ASK block in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != RCURLY)
    {
        free(dialogueName);
        free(dialogueScene->description);
        free(dialogueScene);

        ParseError("Missing } in dialogue definition.", linenum);
        return false;
    }

    if(!setItem(character->dialogueScenes, dialogueName, dialogueScene))
    {
        free(dialogueName);
        free(dialogueScene->description);
        free(dialogueScene);

        ParseError("Failed to add dialogue scene in Character's dialogue scenes.", linenum);
        return false;
    }

    return true;
}

bool AskBlock(FILE* input, int* linenum, Scene* scene)
{
    LexItem tok = getNextProgToken(input, linenum);

    switch(tok.token)
    {
        case STRING:
            scene->prompt = tok.lexeme;

            tok = getNextProgToken(input, linenum);
            free(tok.lexeme);

            if(tok.token != LCURLY)
            {
                free(scene->prompt);
                ParseError("Missing { in Ask block.", linenum);
                return false;
            }

            break;

        case LCURLY:
            free(tok.lexeme);

            scene->prompt = malloc(strlen("What will you say?") + 1);

            if(!scene->prompt)
            {
                ParseError("Failed to allocate memory for scene prompt.", linenum);
                return false;
            }

            strcpy(scene->prompt, "What will you say?");
            break;

        default:
            ParseError("Missing Prompt in scene.", linenum);
            return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != CHOICE)
    {
        free(scene->prompt);
        ParseError("Must provide at least one choice.", linenum);
        return false;
    }

    scene->choices = createLinkedList();

    if(!scene->choices)
    {
        free(scene->prompt);
        ParseError("Failed to allocate memory for choices.", linenum);
        return false;
    }

    scene->conditional = NULL;

    while(tok.token == CHOICE)
    {
        if(!ChoiceDefinition(input, linenum, scene)) 
        {
            free(scene->prompt);
            freeLinkedList(scene->choices);
            return false;
        }

        tok = getNextProgToken(input, linenum);
        free(tok.lexeme);
    }
    
    if(tok.token != RCURLY)
    {
        free(scene->prompt);
        freeLinkedList(scene->choices);

        ParseError("Missing } in Ask Block.", linenum);
        return false;
    }

    return true;
}

bool ChoiceDefinition(FILE* input, int* linenum, Scene* scene)
{
    bool status = false;
    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        free(tok.lexeme);
        ParseError("Missing choice description in Choice.", linenum);
        return status;
    }

    Choice* choice = malloc(sizeof(Choice));

    if(!choice)
    {
        free(tok.lexeme);
        ParseError("Failed to allocate memory for Choice.", linenum);
        return false;
    }

    choice->choiceDescription = tok.lexeme;

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != ARROW)
    {
        free(choice->choiceDescription);
        free(choice);

        ParseError("Missing -> in Choice.", linenum);
        return status;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    switch(tok.token)
    {
        case EFFECT:
            status = EffectDefinition(input, linenum, &choice->effect);
            break;
        case IF:
            status = IfBlock(input, linenum, &choice->conditional);
            break;
        default:
            ParseError("Must have an effect for choice.", linenum);
            return false;
    }

    bool inserted = true;
    if(!status || !(status = inserted = insertEnd(scene->choices, choice))) 
    {
        if(!inserted) ParseError("Failed to insert choice in scene's choices.", linenum);
        free(choice->choiceDescription);
        free(choice);
    }
    return status;
}

bool IfBlock(FILE* input, int* linenum, ConditionalStmt* conditional)
{
    LexItem tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != PLAYER)
    {
        ParseError("Missing reference to player in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != DOT)
    {
        ParseError("Missing '.' directive after player in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != DOT)
    {
        ParseError("Missing '.' directive after player in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != HAS)
    {
        ParseError("Missing 'has' function call after player in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        free(tok.lexeme);
        ParseError("Missing item name for conditional in If.", linenum);
        return false;
    }

    conditional->itemCondition = tok.lexeme;

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != LCURLY)
    {
        free(conditional->itemCondition);
        ParseError("Missing { in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != EFFECT)
    {
        free(conditional->itemCondition);
        ParseError("Must have an effect for if condition.", linenum);
        return false;
    } 

    if(!EffectDefinition(input, linenum, &conditional->ifEffect))
    {
        free(conditional->itemCondition);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != RCURLY)
    {
        free(conditional->itemCondition);
        ParseError("Missing } in If.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != ELSE)
    {
        free(conditional->itemCondition);
        ParseError("Missing else in If block.", linenum);
        return false;
    }

    if(tok.token != LCURLY)
    {
        free(conditional->itemCondition);
        ParseError("Missing { in else.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != EFFECT)
    {
        free(conditional->itemCondition);
        ParseError("Must have an effect for else condition.", linenum);
        return false;
    } 

    if(!EffectDefinition(input, linenum, &conditional->elseEffect))
    {
        free(conditional->itemCondition);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != RCURLY)
    {
        free(conditional->itemCondition);
        ParseError("Missing } in else.", linenum);
        return false;
    }
}

bool EffectDefinition(FILE* input, int* linenum, Effect* effect)
{
    
}

bool SceneDefinition(FILE* input, int* linenum)
{

}
