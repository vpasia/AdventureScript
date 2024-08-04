#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
    ITEM = 1, SCENE, DESCRIBE,
    ASK, CHOICE, IF, ELSE,
    PLAYER, DOT, RECEIVE, HAS, 
    EFFECT, CHARACTER, STRING, 
    DIALOGUE, SAY, ARROW,
    LCURLY, RCURLY, 
    LPAREN, RPAREN, 
    ERR, DONE
} Token;

typedef struct
{
    Token token;
    char* lexeme;
    int linenum;
} LexItem;

typedef struct
{
    char* text;
    int index;
    size_t length;
} Lexeme;

LexItem getNextToken(FILE* input, int* linenum);

#endif
