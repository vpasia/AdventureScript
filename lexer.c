#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "lexer.h"
#include "map.h"
#include "utils.h"

typedef enum { SSTART, INID, INSTRING, INCOMMENT } TokenState;

static Map* keywords = NULL;
static Map* delimiters = NULL;

bool InitializeMaps()
{
    bool isAdded;

    if(keywords == NULL && delimiters == NULL)
    {
        keywords = createMap();
        delimiters = createMap();

        if(keywords == NULL || delimiters == NULL) return false;

        isAdded = setItem(keywords, "item", (void*)ITEM, true)
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


LexItem getNextToken(Scanner* scanner, int* linenum)
{
    TokenState state = SSTART;
    Lexeme lexeme = {scanner->scanner, 0};

    char checkBuf[11];

    if(!InitializeMaps()) return (LexItem){ERR, "Failed To Initialize Token Maps.", *linenum};

    while(scanner->scanner != scanner->end)
    {
        switch(state)
        {
            case SSTART:
                if(*(scanner->scanner) == '\n' && lexeme.length == 0)
                {
                    lexeme.start = scanner->scanner;
                    (*linenum)++;
                    continue;
                }
                else if(isspace(*(scanner->scanner)) && lexeme.length == 0)
                {
                    lexeme.start = scanner->scanner;
                    continue;
                }

                lexeme.length++;

                if(strcmp(scanner->scanner, "#") == 0)
                {
                    state = INCOMMENT;
                    continue;
                }
                else if(isalpha(*(scanner->scanner)))
                {
                    state = INID;
                    continue;
                }
                else if(strcmp(scanner->scanner,"\"") == 0)
                {
                    state = INSTRING;
                    continue;
                }

                if(lexeme.length > 3)
                {
                    return (LexItem) {ERR, lexeme, *linenum};                                        
                }

                sprintf(checkBuf, "%.*s", lexeme.length, lexeme.start);

                int* posDelim = getItem(delimiters, checkBuf);

                if(posDelim)
                {
                    Token token = (Token)(intptr_t)posDelim;
                    return (LexItem) {token, lexeme, *linenum};
                }

                break;
            
            case INID:
                if(!isalpha(*(scanner->scanner)))
                {
                    scanner->scanner--;

                    sprintf(checkBuf, "%.*s", lexeme.length, lexeme.start);
                    int* posKeyword = getItem(keywords, checkBuf);

                    if(posKeyword)
                    {
                        Token token = (Token)(intptr_t)posKeyword;
                        return (LexItem) {token, lexeme, *linenum};
                    }
                    else
                    {
                        return (LexItem){ERR, lexeme, *linenum};
                    }
                }
                
                lexeme.length++;

                break;
            
            case INSTRING:
                if(*(scanner->scanner) == '\n')
                {
                    return (LexItem) {ERR, lexeme, *linenum};
                }

                lexeme.length++;

                if(*(scanner->scanner) == '"')
                {
                    lexeme.start++;
                    lexeme.length--;

                    return (LexItem){STRING, lexeme, *linenum};
                }
                break;
            
            case INCOMMENT:
                if(*(scanner->scanner) == '\n')
                {
                    state = SSTART;
                    lexeme.start = scanner->scanner + 1;
                    lexeme.length = 0;

                    (*linenum)++;
                }
                break;
        }
        scanner->scanner++;
    }

    freeTokenMaps();

    if(scanner->scanner == scanner->end)
    {
        return (LexItem){DONE, "Finished", *linenum};
    }
    
    return (LexItem){ERR, "IO Error", *linenum};
}
