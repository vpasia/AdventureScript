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

bool addCharToLexeme(char** lexeme, int* lexemeIndex, char character)
{
    if((*lexeme)[(*lexemeIndex)] == '\0')
    {
        char* tmp = realloc(*lexeme, (*lexemeIndex) + 2);

        if(tmp != NULL)
        {
            *lexeme = tmp;
        }
        else
        {
            return false;
        }
    }

    (*lexeme)[(*lexemeIndex)++] = character;
    (*lexeme)[*lexemeIndex] = '\0';
    return true;
}

LexItem getNextToken(FILE* input, int* linenum)
{
    if(!InitializeMaps()) return (LexItem){ERR, "Failed To Initialize Token Maps.", *linenum};

    TokenState state = START;

    char* lexeme = malloc(1);
    lexeme[0] = '\0';

    int lexemeIdx = 0;
    char ch;

    while((ch = fgetc(input)) != EOF)
    {
        switch(state)
        {
            case START:
                if(ch == '\n' && strlen(lexeme) == 0)
                {
                    (*linenum)++;
                    continue;
                }
                else if(isspace(ch) && strlen(lexeme) == 0)
                {
                    continue;
                }

                if(!addCharToLexeme(&lexeme, &lexemeIdx, ch))
                {
                    freeTokenMaps();
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(strcmp(lexeme, "#") == 0)
                {
                    state = INCOMMENT;
                    continue;
                }
                else if(isalpha(ch))
                {
                    state = INID;
                    continue;
                }
                else if(strcmp(lexeme,"\"") == 0)
                {
                    state = INSTRING;
                    continue;
                }

                int* posDelim = (int*)getItem(delimiters, lexeme);
                if(posDelim != NULL)
                {
                    Token token = (Token)(intptr_t)posDelim;
                    return (LexItem) {token, lexeme, *linenum};
                }

                break;
            
            case INID:
                if(!isalpha(ch))
                {
                    ungetc(ch, input);

                    int* posKeyword = (int*)getItem(keywords, lexeme);

                    if(posKeyword != NULL)
                    {
                        Token token = (Token)(intptr_t)posKeyword;
                        return (LexItem) {token, lexeme, *linenum};
                    }
                    else
                    {
                        return (LexItem){ERR, lexeme, *linenum};
                    }
                }
                
                if(!addCharToLexeme(&lexeme, &lexemeIdx, ch))
                {
                    freeTokenMaps();
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                break;
            
            case INSTRING:
                if(ch == '\n')
                {
                    freeTokenMaps();
                    return (LexItem) {ERR, lexeme, *linenum};
                }

                if(!addCharToLexeme(&lexeme, &lexemeIdx, ch))
                {
                    freeTokenMaps();
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(ch == '"')
                {
                    return (LexItem){STRING, lexeme, *linenum};
                }
                break;
            
            case INCOMMENT:
                if(ch == '\n')
                {
                    state = START;
                    free(lexeme);
                    lexeme = malloc(1);
                    lexeme[0] = '\0';

                    lexemeIdx = 0;
                    (*linenum)++;
                }
                break;
        }
    }

    if(feof(input))
    {
        freeTokenMaps();
        return (LexItem){DONE, "Finished", *linenum};
    }

    freeTokenMaps();
    return (LexItem){ERR, "IO Error", *linenum};
}
