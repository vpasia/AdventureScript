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

	m->values = calloc(INITIAL_MAP_CAPACITY, sizeof(MapValue));
	if(m->values == NULL)
	{
		free(m);
		return NULL;
	}

	return m;
}

void freeMap(Map* map)
{
	free(map->values);
	free(map);
}

bool setItem(Map* map, const char* key, void* value)
{
	if(map->count == map->capacity)
	{
		size_t newSize = sizeof(MapValue) * (map->capacity) * 2;
		MapValue* tmp = realloc(map->values, newSize);

		if(tmp != NULL)
		{
			map->values = tmp;
			map->capacity *= 2;
		}
		else
		{
			printf("Failed to Resize Map");
			return false;
		}

	}


	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	if(map->values[index].value == NULL) map->count++;
	map->values[index].value = value;
	return true;
}

void* getItem(Map* map, const char* key)
{
	uint64_t hash = hash_key(key);
	size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

	return map->values[index].value;
}

