#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* tokenToString(Token token) 
{
    switch (token) 
    {
        case ITEM: return "ITEM";
        case SCENE: return "SCENE";
        case DESCRIBE: return "DESCRIBE";
        case ASK: return "ASK";
        case CHOICE: return "CHOICE";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case PLAYER: return "PLAYER";
        case RECEIVE: return "RECEIVE";
        case HAS: return "HAS";
        case EFFECT: return "EFFECT";
        case CHARACTER: return "CHARACTER";
        case DIALOGUE: return "DIALOGUE";
        case SAY: return "SAY";
        case DOT: return ".";
        case LCURLY: return "{";
        case RCURLY: return "}";
        case LPAREN: return "(";
        case RPAREN: return ")";
        case ARROW: return "->";
        case DONE: return "DONE";
        case STRING: return "STRING";
        case NOT: return "NOT";
        default: return "ERR";
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("NO SPECIFIED INPUT FILE. \n");
        exit(1);
    }

    char* fileExtension = strrchr(argv[1], '.');

    if(strcmp(fileExtension, ".adv") != 0)
    {
        printf("INVALID PROGRAM FILE INPUTTED. \n");
        free(fileExtension);
        exit(1);
    }

    free(fileExtension);

    FILE* inputFile = fopen(argv[1], "r");

    if(inputFile == NULL)
    {
        printf("CANNOT OPEN THE FILE %s \n", argv[1]);
        exit(1);
    }

    int linenum = 0;

    bool status = Prog(inputFile, &linenum);

    status ? printf("Successful Parsing.\n") : printf("Unsuccessful Parsing.\n");

    fclose(inputFile);

    return 0;
}
