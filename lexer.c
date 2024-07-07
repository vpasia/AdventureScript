#include "lexer.h"
#include "map.h"

#include <ctype.h>
#include <stdbool.h>

typedef enum { START, INID, INSTRING, INCOMMENT } TokenState;

Map* keywords;
Map* delimiters;

void InitializeMaps()
{
    if(keywords == NULL && delimiters == NULL)
    {
        keywords = createMap();
        delimiters = createMap();
    }
}


bool addCharToLexeme(char* lexeme, int* lexemeIndex, char character)
{
    size_t currentSize = strlen(lexeme);

    if(currentSize == (*lexemeIndex))
    {
        char* tmp = realloc(lexeme, (currentSize * 2) + 1);

        if(tmp != NULL)
        {
            lexeme = tmp;
        }
        else
        {
            return false;
        }
    }

    lexeme[(*lexemeIndex)++] = character;
    return true;
}

void refitLexemeBuffer(char* buffer)
{
    size_t actualSize = strlen(buffer) + 1;
    char* tmp = realloc(buffer, actualSize);

    if(tmp != NULL)
    {
        buffer = tmp;
    }
}

LexItem getNextToken(FILE* input, int* linenum)
{
    InitializeMaps();

    TokenState state = START;

    char* lexeme = malloc(5);
    int lexemeIdx = 0;
    char ch;

    while(ch = fgetc(input))
    {
        switch(state)
        {
            case START:
                if(ch == '\n' && lexeme == NULL)
                {
                    (*linenum)++;
                    continue;
                }
                else if(isspace(ch) && lexeme == NULL)
                {
                    continue;
                }

                if(!addCharToLexeme(lexeme, &lexemeIdx, ch))
                {
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
                else if(strcmp(lexeme, "'") == 0 || strcmp(lexeme,"\"") == 0)
                {
                    state = INSTRING;
                    continue;
                }

                if(strcmp(lexeme, "-") == 0)
                {
                    continue;    
                }
                else if(strcmp(lexeme, "->") == 0)
                {
                    refitLexemeBuffer(lexeme);
                    return (LexItem) {ARROW, lexeme, *linenum};
                }

                break;
            case INID:
                break;
            case INSTRING:
                if(ch == '\n') return (LexItem) {ERR, lexeme, *linenum};

                if(!addCharToLexeme(lexeme, &lexemeIdx, ch))
                {
                    return (LexItem){ERR, "Unable to Reallocate Memory for Lexeme", *linenum};
                }

                if(ch == '"' || ch == '\'')
                {
                    refitLexemeBuffer(lexeme);
                    return lexeme[0] == ch ? (LexItem){STRING, lexeme, *linenum} : (LexItem){ERR, lexeme, *linenum};
                }
                break;
            case INCOMMENT:
                if(ch == '\n')
                {
                    free(lexeme);
                    lexeme = malloc(5);
                    lexemeIdx = 0;
                    (*linenum)++;
                }
                break;
        }
    }
}