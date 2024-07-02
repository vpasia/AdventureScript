#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
    ITEM, SCENE, DESCRIBE,
    ASK, CHOICE, IF, ELSE,
    HAS_CONDITION, EFFECT, CHARACTER,
    IDENT, STRING, NUMBER, DIALOGUE,
    SAY
} Token;

typedef struct LexItem
{
    Token token;
    char lexeme[128];
    int lineNum;
} LexItem;

LexItem getNextToken(FILE* input, int* linenum);

#endif