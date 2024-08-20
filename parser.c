#include "parser.h"
#include "map.h"

Map* scenes = NULL;
Map* playerInventory = NULL;

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

bool Prog(FILE* input, int* linenum)
{
    bool dls = false, scd = false;
    LexItem tok = getNextProgToken(input, linenum);

    if(tok.token == ERR)
    {
        ParseError("Unrecognized Input Pattern", linenum);
        return false;
    }
    else if(tok.token != ITEM || tok.token != CHARACTER || tok.token != SCENE)
    {
        ParseError("Must Define Elements of Story First!", linenum);
        return false;
    }

    dls = Decl(input, linenum);
    if(!dls)
    {
        ParseError("Incorrect Declaration in Program", linenum);
        return false;
    }

    scd = SceneDefinition(input, linenum);
    if(!scd)
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

}


bool SceneDefinition(FILE* input, int* linenum)
{

}
