#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int indexOf(char* str, char ch)
{
    char* pos = strstr(str, ch);
    return pos ? pos - str : -1;
}

char* substring(char* str, int start, int end)
{
    int len = end - start;
    char* substr = malloc(len + 1);

    int i = 0;
    for(i; i < len; i++)
    {
        substr[i] = str[start + i];
    }

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



    return 0;
}
