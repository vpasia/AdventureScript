#include "lexer.h"

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
        default: return "ERR";
    }
}

int indexOf(char* str, char ch)
{
    char* pos = strchr(str, ch);
    return pos ? pos - str : -1;
}

char* substring(char* str, int start, int end)
{
    int len = end - start;
    char* substr = malloc(len + 1);

    int i;
    for(i = 0; i < len; i++)
    {
        substr[i] = str[start + i];
    }

    substr[len] = '\0';

    return substr;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("NO SPECIFIED INPUT FILE. \n");
        exit(1);
    }

    char* fileExtension = substring(argv[1], indexOf(argv[1], '.'), strlen(argv[1]));

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

    LexItem tok;
    int linenum = 1;
    printf("Starting Lexing...\n");
    while((tok = getNextToken(inputFile, &linenum)).token != DONE && tok.token != ERR)
    {
        printf("%d: %s -> %s \n", linenum, tok.lexeme, tokenToString(tok.token));
        free(tok.lexeme);
    }

    if(tok.token == DONE)
    {
        printf("%d: %s \n", tok.linenum, tok.lexeme);
        printf("Successfully Processed Lexemes \n");

    }
    else
    {
        printf("%d: %s -> %s \n", linenum, tok.lexeme, tokenToString(tok.token));
    }

    free(tok.lexeme);

    fclose(inputFile);

    return 0;
}
