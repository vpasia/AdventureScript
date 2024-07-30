#include "map.h"

#include <string.h>


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
			free((void*)previousEntry->key);
			free(previousEntry);
		}
	}

	free(map->entries);
	free(map);
}

bool resizeMap(Map* map)
{
	size_t newCapacity = map->capacity * 2;
	MapEntry* newEntries = calloc(newCapacity, sizeof(MapEntry));

	if(newEntries == NULL) return false;

	int i;
	for(i = 0; i < map->capacity; i++)
	{
		MapEntry* entry = &map->entries[i];

		uint64_t hash = hash_key(entry->key);
		size_t newIndex = (size_t)(hash & (uint64_t)(newCapacity - 1));

		MapEntry* newEntry = &newEntries[newIndex];

		while(entry != NULL)
		{
			newEntry->key = entry->key;
			newEntry->value = entry->value;
			newEntry->next = entry->next;

			newEntry = newEntry->next;
			entry = entry->next;
		}
	}

	free(map->entries);
	map->entries = newEntries;
	map->capacity = newCapacity;

	return true;
}

bool setItem(Map* map, const char* key, void* value)
{
	if(map->count == map->capacity && !resizeMap(map)) return false;

	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	if(map->entries[index].key == NULL) map->count++;

	MapEntry* currentEntry = &map->entries[index];
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

	return true;
}

void* getItem(Map* map, const char* key)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	MapEntry* currentEntry = &map->entries[index];
	printf("%s - %s at index %zu with pointer %p\n", currentEntry->key, key, index, currentEntry);

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
