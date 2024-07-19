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

	return m;
}

void freeMap(Map* map)
{
	free(map->entries);
	free(map);
}

bool setItem(Map* map, const char* key, void* value)
{
	if(map->count == map->capacity)
	{
		size_t newSize = sizeof(MapEntry) * (map->capacity) * 2;

		MapEntry* tmpEntries = realloc(map->entries, newSize);

		if(tmpEntries != NULL)
		{
			map->entries = tmpEntries;
			map->capacity *= 2;
		}
		else
		{
			return false;
		}
	}


	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	if(map->entries[index].key == NULL) map->count++;

	map->entries[index].key = strdup(key);
	map->entries[index].value = value;
	return true;
}

void* getItem(Map* map, const char* key)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	char* retrievedKey = (char*)map->entries[index].key;

	if(retrievedKey == NULL)
	{
		return NULL;
	}

	return map->entries[index].value;
}
