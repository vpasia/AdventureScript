#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "linkedlist.h"

LinkedList* createLinkedList()
{
	LinkedList* list = malloc(sizeof(LinkedList));

	if(!list) return NULL;

	list->head = NULL;
	list->tail = NULL;

	return list;
}

void freeLinkedList(LinkedList* list)
{
	Node* previous = list->head;
	Node* current = previous;

	while(current)
	{
		previous = current;
		current = current->next;

		free(previous->value);
		free(previous);
	}

	list->head = NULL;
	free(list);
}

bool insertEnd(LinkedList* list, void* value)
{
	Node* node = malloc(sizeof(Node));

	if(!node) return false;

	node->value = value;
	node->next = NULL;

	if(!list->head)
	{
		list->head = node;
	}
	else if(!list->head->next)
	{
		list->head->next = node;
		list->tail = list->head->next;
	}
	else
	{
		list->tail->next = node;
		list->tail = list->tail->next;
	}

	return true;
}

bool removeListItem(LinkedList* list, void* value, int (*comparator)(void*, void*))
{
	Node* previous = list->head;
	Node* current = previous;

	if(previous && (*comparator)(previous->value, value) == 0)
	{
		list->head = previous->next;
		free(previous->value);
		free(previous);

		return true;
	}

	while(current && (*comparator)(current->value, value) != 0)
	{
		previous = current;
		current = current->next;
	}

	if(!current) return false;

	previous->next = current->next;

	if(current == list->tail) list->tail = previous;

	free(current->value);
	free(current);

	return true;
}