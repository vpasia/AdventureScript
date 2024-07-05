#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
    ITEM, SCENE, DESCRIBE,
    ASK, CHOICE, IF, ELSE,
    PLAYER, DOT, RECEIVE, HAS, 
    EFFECT, CHARACTER,
    IDENT, STRING, DIALOGUE,
    SAY, ERR, LCURLY, RCURLY, ARROW,
    LPAREN, RPAREN
} Token;

typedef struct
{
    Token token;
    char* lexeme;
    int lineNum;
} LexItem;

LexItem getNextToken(FILE* input, int* linenum);

#endif
