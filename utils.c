#include <stdlib.h>
#include <string.h>

#include "utils.h"

char* strdupl(const char* s) 
{
	size_t len = strlen(s) + 1;
	char* copy = malloc(len);
	if (copy) 
	{
		memcpy(copy, s, len);
	}
	return copy;
}

char* substring(char* str, int start, int end)
{
    int len = end - start;
    char* substr = malloc(len + 1);

    int i;
    for(i = 0; i < len; i++)
    {
        substr[i] = str[start + i];
    }

    substr[len] = '\0';

    return substr;
}
