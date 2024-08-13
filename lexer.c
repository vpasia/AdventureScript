#include "lexer.h"
#include "map.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

typedef enum { START, INID, INSTRING, INCOMMENT } TokenState;

Map* keywords = NULL;
Map* delimiters = NULL;

bool InitializeMaps()
{
    if(keywords == NULL && delimiters == NULL)
    {
        keywords = createMap();
        delimiters = createMap();

        bool isAdded = setItem(keywords, "item", (void*)ITEM)
                        && setItem(keywords, "scene", (void*)SCENE)
                        && setItem(keywords, "describe", (void*)DESCRIBE)
                        && setItem(keywords, "ask", (void*)ASK)
                        && setItem(keywords, "choice", (void*)CHOICE)
                        && setItem(keywords, "if", (void*)IF)
                        && setItem(keywords, "else", (void*)ELSE)
                        && setItem(keywords, "player", (void*)PLAYER)
                        && setItem(keywords, "receive", (void*)RECEIVE)
                        && setItem(keywords, "has", (void*)HAS)
                        && setItem(keywords, "effect", (void*)EFFECT)
                        && setItem(keywords, "character", (void*)CHARACTER)
                        && setItem(keywords, "dialogue", (void*)DIALOGUE)
                        && setItem(keywords, "say", (void*)SAY);
        
        if(!isAdded) return isAdded;

        isAdded = setItem(delimiters, ".", (void*)DOT)
                    && setItem(delimiters, "{", (void*)LCURLY)
                    && setItem(delimiters, "}", (void*)RCURLY)
                    && setItem(delimiters, "(", (void*)LPAREN)
                    && setItem(delimiters, ")", (void*)RPAREN)
                    && setItem(delimiters, "->", (void*)ARROW);
        
        return isAdded;
    }

    return true;
}

void freeTokenMaps()
{
    freeMap(keywords);
    freeMap(delimiters);
}

bool addCharToLexeme(Lexeme* lexeme, char character)
{
    if(lexeme->index + 1 >= lexeme->length)
    {
        char* tmp = realloc(lexeme->text, lexeme->length * 2);

        if(tmp != NULL)
        {
            lexeme->text = tmp;
            lexeme->length *= 2;
        }
        else
        {
            return false;
        }
    }

    lexeme->text[lexeme->index++] = character;
    lexeme->text[lexeme->index] = '\0';
    return true;
}

bool refitLexemeBuffer(Lexeme* buffer)
{
    char* pos = strchr(buffer->text, '\0');
    int end = pos ? pos - buffer->text : -1;

    if(end < 0) return false;

    char* tmp = malloc(end + 1);
    if(!tmp) return false;

    strcpy(tmp, buffer->text);

    free(buffer->text);
    buffer->text = tmp;
    buffer->length = end + 1;

    return true;
}

LexItem getNextToken(FILE* input, int* linenum)
{
    if(!InitializeMaps()) return (LexItem){ERR, "Failed To Initialize Token Maps.", *linenum};

    TokenState state = START;

    Lexeme lexeme = {malloc(2), 0, 2};
    if (lexeme.text == NULL) return (LexItem){ERR, "Memory Allocation failed for Lexeme", *linenum};
    lexeme.text[0] = '\0';

    char ch;

    while((ch = fgetc(input)) != EOF)
    {
        switch(state)
        {
            case START:
                if(ch == '\n' && lexeme.index == 0)
                {
                    (*linenum)++;
                    continue;
                }
                else if(isspace(ch) && lexeme.index == 0)
                {
                    continue;
                }

                if(!addCharToLexeme(&lexeme, ch))
                {
                    free(lexeme.text);
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(strcmp(lexeme.text, "#") == 0)
                {
                    state = INCOMMENT;
                    continue;
                }
                else if(isalpha(ch))
                {
                    state = INID;
                    continue;
                }
                else if(strcmp(lexeme.text,"\"") == 0)
                {
                    state = INSTRING;
                    continue;
                }

                if(!refitLexemeBuffer(&lexeme))
                {
                    free(lexeme.text);
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                int* posDelim = (int*)getItem(delimiters, lexeme.text);
                if(posDelim != NULL)
                {
                    Token token = (Token)(intptr_t)posDelim;
                    return (LexItem) {token, lexeme.text, *linenum};
                }

                break;
            
            case INID:
                if(!isalpha(ch))
                {
                    ungetc(ch, input);

                    if(!refitLexemeBuffer(&lexeme))
                    {
                        free(lexeme.text);
                        return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                    }

                    int* posKeyword = (int*)getItem(keywords, lexeme.text);

                    if(posKeyword != NULL)
                    {
                        Token token = (Token)(intptr_t)posKeyword;
                        return (LexItem) {token, lexeme.text, *linenum};
                    }
                    else
                    {
                        return (LexItem){ERR, lexeme.text, *linenum};
                    }
                }
                
                if(!addCharToLexeme(&lexeme, ch))
                {
                    free(lexeme.text);
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                break;
            
            case INSTRING:
                if(ch == '\n')
                {
                    if(!refitLexemeBuffer(&lexeme))
                    {
                        free(lexeme.text);
                        return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                    }
                    return (LexItem) {ERR, lexeme.text, *linenum};
                }

                if(!addCharToLexeme(&lexeme, ch))
                {
                    free(lexeme.text);
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(ch == '"')
                {
                    if(!refitLexemeBuffer(&lexeme))
                    {
                        free(lexeme.text);
                        return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                    }
                    return (LexItem){STRING, lexeme.text, *linenum};
                }
                break;
            
            case INCOMMENT:
                if(ch == '\n')
                {
                    state = START;
                    free(lexeme.text);
                    lexeme.text = malloc(2);
                    lexeme.text[1] = '\0';

                    lexeme.index = 0;
                    lexeme.length = 2;
                    (*linenum)++;
                }
                break;
        }
    }

    free(lexeme.text);
    freeTokenMaps();

    if(feof(input))
    {
        return (LexItem){DONE, "Finished", *linenum};
    }
    
    return (LexItem){ERR, "IO Error", *linenum};
}
