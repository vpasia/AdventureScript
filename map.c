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

	m->keys = calloc(INITIAL_MAP_CAPACITY, sizeof(MapEntry));
	m->values = calloc(INITIAL_MAP_CAPACITY, sizeof(MapEntry));
	if(m->keys == NULL || m->values == NULL)
	{
		freeMap(m);
		return NULL;
	}

	return m;
}

void freeMap(Map* map)
{
	free(map->keys);
	free(map->values);
	free(map);
}

bool setItem(Map* map, const char* key, void* value)
{
	if(map->count == map->capacity)
	{
		size_t newSize = sizeof(MapEntry) * (map->capacity) * 2;

		MapEntry* tmpKeys = realloc(map->keys, newSize);
		MapEntry* tmpVals = realloc(map->values, newSize);

		if(tmpVals != NULL && tmpKeys != NULL)
		{
			free(map->keys);
			free(map->values);

			map->keys = tmpKeys;
			map->values = tmpVals;
			map->capacity *= 2;
		}
		else
		{
			return false;
		}
	}


	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	if(map->values[index].value == NULL) map->count++;
	map->keys[index].value = (void*)strdup(key);
	map->values[index].value = value;
	return true;
}

void* getItem(Map* map, const char* key)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	char* retrievedKey = (char*)map->keys[index].value;

	if(strcmp(key, retrievedKey) != 0)
	{
		return (void*)(-1);
	}

	return map->values[index].value;
}
