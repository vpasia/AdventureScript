#include "parser.h"
#include "map.h"

#define ParseError(message, linenum) printf("%d: %s \n", linenum, message);

Map* scenes = NULL;
Map* playerInventory = NULL;
Map* items = NULL;
Map* characters = NULL;

pushedBack = false;
LexItem pushedBackToken;

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
        ParseError("Missing Item name in Declaration.", linenum);
        free(tok.lexeme);
        return false;
    }

    if(!setItem(items, tok.lexeme, true))
    {
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
        ParseError("Missing Character Name in Definition.", linenum);
        return false;
    }

    char* characterName = tok.lexeme;
    Character* character = malloc(sizeof(Character));

    if(!character)
    {
        ParseError("Unable to allocate memory for character.", linenum);
        free(characterName);
        return false;
    }

    character->dialogueScenes = createMap();

    if(!character->dialogueScenes)
    {
        ParseError("Unable to allocate memory for character dialogue scenes.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != LCURLY)
    {
        ParseError("Missing { in Character Definition.", linenum);
        return false;
    }
    
    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != DIALOGUE)
    {
        ParseError("Must at least define one dialogue scene.", linenum);
        return false;
    }

    while(tok.token == DIALOGUE)
    {
        if(!DialogueContent(input, linenum, character)) return false;
        tok = getNextProgToken(input, linenum);
        free(tok.lexeme);
    }

    if(tok.token != RCURLY)
    {
        ParseError("Missing } in Character Definition.", linenum);
        return false;
    }

    if(!setItem(characters, characterName, character))
    {
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
        ParseError("Missing Dialogue name in Dialogue definition.", linenum);
        free(tok.lexeme);
        return false;
    }

    char* dialogueName = tok.lexeme;
    Scene* dialogueScene = malloc(sizeof(Scene));

    if(!dialogueScene)
    {
        ParseError("Failed to allocate memory for dialogue scene.", linenum);
        return false;
    }

    dialogueScene->description = NULL;

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != LCURLY)
    {
        ParseError("Missing { in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != SAY) 
    {
        ParseError("Missing SAY keyword in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);

    if(tok.token != STRING)
    {
        ParseError("Missing Character Dialogue in Dialogue definition.", linenum);
        free(tok.lexeme);
        return false;
    }

    dialogueScene->prompt = tok.lexeme;

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != ASK)
    {
        ParseError("Missing ASK block in Dialogue definition.", linenum);
        return false;
    }

    if(!AskBlock(input, linenum, dialogueScene))
    {
        ParseError("Incorrect ASK block in Dialogue definition.", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);
    free(tok.lexeme);

    if(tok.token != RCURLY)
    {
        ParseError("Missing } in dialogue definition.", linenum);
        return false;
    }

    if(!setItem(character->dialogueScenes, dialogueName, dialogueScene))
    {
        ParseError("Failed to add dialogue scene in Character's dialogue scenes.", linenum);
        return false;
    }

    return true;
}

bool AskBlock(FILE* input, int* linenum, Scene* scene)
{
    
}

bool SceneDefinition(FILE* input, int* linenum)
{

}
