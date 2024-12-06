#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

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
        case START: return "START";
        case END: return "END";
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
        exit(1);
    }

    FILE* inputFile = fopen(argv[1], "r");

    if(inputFile == NULL)
    {
        printf("CANNOT OPEN THE FILE %s \n", argv[1]);
        exit(1);
    }

    int linenum = 1;

    /*
    LexItem tok;
    printf("Starting Lexing...\n");
    
    while((tok = getNextToken(inputFile, &linenum)).token != DONE && tok.token != ERR)
    {
        printf("%d: %s -> %s \n", linenum, tok.lexeme, tokenToString(tok.token));
        free(tok.lexeme);
    }

    if(tok.token == DONE)
    {
        printf("Successfully Processed Lexemes \n");
    }
    else
    {
        printf("%d: %s -> %s \n", linenum, tok.lexeme, tokenToString(tok.token));
    }
    */
    

    
    bool status = Prog(inputFile, &linenum);

    if(status)
    {
        printf("Successful Parsing.\n");
    }
    else
    {
        printf("Unsuccessful Parsing.\n");
    }    
    

    fclose(inputFile);

    return 0;
}
