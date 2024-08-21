#include "parser.h"
#include "map.h"

Map* scenes = NULL;
Map* playerInventory = NULL;
Map* declaredItems = NULL;
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

int errorCount = 0;

void ParseError(const char* message, int linenum)
{
    errorCount++;
    printf("%d: %s \n", errorCount, message);
}

void FreeUtilMaps()
{
    freeMap(scenes);
    freeMap(playerInventory);
    freeMap(declaredItems);
    freeMap(characters);
}

bool Prog(FILE* input, int* linenum)
{
    scenes = createMap();
    playerInventory = createMap();
    declaredItems = createMap();
    characters = createMap();

    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token == ERR)
    {
        char buffer[26 + strlen(tok.lexeme) + 1];
        sprintf(&buffer, "Unrecognized Input Pattern %s", tok.lexeme);
        free(tok.lexeme);

        ParseError(buffer, linenum);
        return false;
    }
    else if(tok.token != ITEM || tok.token != CHARACTER || tok.token != SCENE)
    {
        ParseError("Must Define Elements of Story First!", linenum);
        return false;
    }

    pushBackToken(&tok);

    if(!Decl(input, linenum))
    {
        ParseError("Incorrect Declaration in Program", linenum);
        return false;
    }

    if(!SceneDefinition(input, linenum))
    {
        ParseError("Incorrect Scene Definition in Program", linenum);
        return false;
    }

    tok = getNextProgToken(input, linenum);

    if(tok.token != START)
    {
        ParseError("Missing Entry Point in Program", linenum);
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

    setItem(declaredItems, tok.lexeme, true);
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

    setItem(characters, characterName, character);
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

    
}

bool SceneDefinition(FILE* input, int* linenum)
{

}
