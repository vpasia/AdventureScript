#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
    ITEM = 1, SCENE, DESCRIBE,
    ASK, CHOICE, IF, ELSE, NOT,
    PLAYER, DOT, RECEIVE, HAS, 
    EFFECT, CHARACTER, STRING, 
    DIALOGUE, SAY, ARROW,
    LCURLY, RCURLY, 
    LPAREN, RPAREN, 
    ERR, DONE, START, END
} Token;

typedef struct
{
    char* scanner;
    char* end;
} Scanner;

typedef struct
{
    char* start;
    size_t length;
} Lexeme;

typedef struct
{
    Token token;
    Lexeme lexeme;
    int linenum;
} LexItem;

LexItem getNextToken(FILE* input, int* linenum);

void freeTokenMaps();

#endif
