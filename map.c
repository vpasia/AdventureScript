#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "map.h"


static uint64_t hash_key(const char* key)
{
	uint64_t hash = FNV_OFFSET;
	const char* p;

	for(p = key; *p; p++)
	{
		hash ^= (uint64_t)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}

	return hash;
}

static char* strdupl(const char* s) 
{
	size_t len = strlen(s) + 1;
	char* copy = malloc(len);
	if (copy) 
	{
		memcpy(copy, s, len);
	}
	return copy;
}


Map* createMap()
{
	Map* m = malloc(sizeof(Map));
	if(m == NULL) return NULL;

	m->capacity = INITIAL_MAP_CAPACITY;
	m->count = 0;

	m->entries = calloc(INITIAL_MAP_CAPACITY, sizeof(MapEntry));

	if(m->entries == NULL)
	{
		freeMap(m);
		return NULL;
	}

	int i;
	for(i = 0; i < m->capacity; i++)
	{
		m->entries[i].key = NULL;
		m->entries[i].next = NULL;
	}

	return m;
}

void freeMap(Map* map)
{
	int i;
	for(i = 0; i < map->capacity; i++)
	{    
		MapEntry* previousEntry = map->entries[i].next;
		MapEntry* currentEntry = previousEntry;

		while(currentEntry != NULL)
		{
			previousEntry = currentEntry;
			currentEntry = currentEntry->next;
			free(previousEntry->key);
			free(previousEntry->value);
			free(previousEntry);
		}
	}

	free(map->entries);
	free(map);
}

bool insertItem(MapEntry* entries, int capacity, int* count, const char* key, void* value)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

	MapEntry* currentEntry = &entries[index];
	MapEntry* previousEntry = NULL;

	while(currentEntry != NULL && currentEntry->key != NULL)
	{
		if(strcmp(currentEntry->key, key) == 0)
		{
			break;
		}
		previousEntry = currentEntry;
		currentEntry = currentEntry->next;
	}

	if(currentEntry == NULL)
	{
		if(previousEntry != NULL)
		{
			currentEntry = malloc(sizeof(MapEntry));
			if(currentEntry == NULL)
			{
				return false;
			}
			previousEntry->next = currentEntry;
		}

		currentEntry->key = strdupl(key);
		if(currentEntry->key == NULL)
		{
			return false;
		}

		currentEntry->value = value;
		currentEntry->next = NULL;
	}
	else
	{
		free((void*)currentEntry->key);
		currentEntry->key = strdupl(key);
		if(currentEntry->key == NULL)
		{
			return false;
		}

		currentEntry->value = value;
	}

	(*count)++;
	return true;
}

bool resizeMap(Map* map)
{
	size_t newCapacity = map->capacity * 2;
	int newCount = 0;
	MapEntry* newEntries = calloc(newCapacity, sizeof(MapEntry));

	if(newEntries == NULL) return false;

	
	int i;
	MapEntry* entry = &map->entries[0];

	while(i < map->capacity)
	{
		if(entry && entry->key)
		{
			if(!insertItem(newEntries, newCapacity, &newCount, entry->key, entry->value))
			{
				free(newEntries);
				return false;
			}				
		}

		entry = entry->next ? entry->next : &map->entries[++i];
	}

	
	for(i = 0; i < map->capacity; i++)
	{    
		MapEntry* previousEntry = map->entries[i].next;
		MapEntry* currentEntry = previousEntry;

		while(currentEntry != NULL)
		{
			previousEntry = currentEntry;
			currentEntry = currentEntry->next;
			free(previousEntry->key);
			free(previousEntry);
		}
	}
	

	free(map->entries);
	map->entries = newEntries;
	map->count = newCount;
	map->capacity = newCapacity;

	return true;
}

bool setItem(Map* map, const char* key, void* value)
{
	if(map->count == map->capacity && !resizeMap(map)) return false;
	
	return insertItem(map->entries, map->capacity, &map->count, key, value);
}

void* getItem(Map* map, const char* key)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	MapEntry* currentEntry = &map->entries[index];

	while(currentEntry != NULL && currentEntry->key != NULL)
	{
		if(strcmp(currentEntry->key, key) == 0)
		{
			return currentEntry->value;
		}

		currentEntry = currentEntry->next;
	}

	return NULL;
}
