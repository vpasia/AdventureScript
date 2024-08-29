#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

typedef struct Node
{
	void* value;
	struct Node* next;
} Node;

typedef struct LinkedList
{
	Node* head;
	Node* tail;
} LinkedList;

LinkedList* createLinkedList();

void freeLinkedList(LinkedList* list);
bool insertEnd(LinkedList* list, void* value);
bool removeListItem(LinkedList* list, void* value, int (*comparator)(void*, void*));

#endif