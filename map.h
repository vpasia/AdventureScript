#ifndef MAP_H
#define MAP_H

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 109911628211UL
#define INITIAL_MAP_CAPACITY 16

typedef struct MapEntry 
{
	char* key;
	void* value;
	struct MapEntry* next;
} MapEntry;

typedef struct 
{
	MapEntry* entries;
	int capacity;
	int count;
} Map;

Map* createMap();
void freeMap(Map* map, void (*freeValue)(void*));

bool setItem(Map* map, const char* key, void* value);
void* getItem(Map* map, const char* key);

#endif
