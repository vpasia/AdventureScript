#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "lexer.h"
#include "map.h"
#include "utils.h"

typedef enum { SSTART, INID, INSTRING, INCOMMENT } TokenState;

Map* keywords = NULL;
Map* delimiters = NULL;

bool InitializeMaps()
{
    if(keywords == NULL && delimiters == NULL)
    {
        keywords = createMap();
        delimiters = createMap();

        if(keywords == NULL || delimiters == NULL) return false;

        bool isAdded = setItem(keywords, "item", (void*)ITEM, true)
                        && setItem(keywords, "scene", (void*)SCENE, true)
                        && setItem(keywords, "describe", (void*)DESCRIBE, true)
                        && setItem(keywords, "ask", (void*)ASK, true)
                        && setItem(keywords, "choice", (void*)CHOICE, true)
                        && setItem(keywords, "if", (void*)IF, true)
                        && setItem(keywords, "else", (void*)ELSE, true)
                        && setItem(keywords, "not", (void*)NOT, true)
                        && setItem(keywords, "player", (void*)PLAYER, true)
                        && setItem(keywords, "receive", (void*)RECEIVE, true)
                        && setItem(keywords, "has", (void*)HAS, true)
                        && setItem(keywords, "effect", (void*)EFFECT, true)
                        && setItem(keywords, "character", (void*)CHARACTER, true)
                        && setItem(keywords, "dialogue", (void*)DIALOGUE, true)
                        && setItem(keywords, "say", (void*)SAY, true)
                        && setItem(keywords, "start", (void*)START, true)
                        && setItem(keywords, "end", (void*)END, true);
        
        if(!isAdded) return isAdded;

        isAdded = setItem(delimiters, ".", (void*)DOT, true)
                    && setItem(delimiters, "{", (void*)LCURLY, true)
                    && setItem(delimiters, "}", (void*)RCURLY, true)
                    && setItem(delimiters, "(", (void*)LPAREN, true)
                    && setItem(delimiters, ")", (void*)RPAREN, true)
                    && setItem(delimiters, "->", (void*)ARROW, true);
        
        return isAdded;
    }

    return true;
}

void freeTokenMaps()
{
    freeMap(keywords, NULL);
    freeMap(delimiters, NULL);
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

    TokenState state = SSTART;

    Lexeme lexeme = {malloc(2), 0, 2};
    if (!lexeme.text) return (LexItem){ERR, "Memory Allocation failed for Lexeme", *linenum};
    lexeme.text[0] = '\0';

    char ch;

    while((ch = fgetc(input)) != EOF)
    {
        switch(state)
        {
            case SSTART:
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

                if(posDelim)
                {
                    Token token = (Token)(intptr_t)posDelim;
                    return (LexItem) {token, lexeme.text, *linenum};
                }
                else if(lexeme.index > 1)
                {
                    return (LexItem) {ERR, lexeme.text, *linenum};
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
                    return (LexItem) {ERR, lexeme.text, *linenum};
                }

                if(!addCharToLexeme(&lexeme, ch))
                {
                    free(lexeme.text);
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(ch == '"')
                {
                    char* actualString = substring(lexeme.text, 1, lexeme.index - 1);
                    free(lexeme.text);

                    return (LexItem){STRING, actualString, *linenum};
                }
                break;
            
            case INCOMMENT:
                if(ch == '\n')
                {
                    state = SSTART;
                    free(lexeme.text);
                    lexeme.text = malloc(2);
                    lexeme.text[0] = '\0';

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
