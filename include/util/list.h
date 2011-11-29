#ifndef LIST_H_
#define LIST_H_

#include <defs.h>

struct node {
	void* element;
	struct node* next;
};

typedef struct node node_t;

typedef struct {
	char name[16];
	node_t* first;
	u32int size;
} List;

void list_init(List* list, char* name);

void list_add(List* list, void* elem);

void* list_remove(List* list, int index);

boolean list_removeElem(List* list, void* elem, int (cmp)(void*, void*));

void* list_get(List* list, int index);

boolean list_isEmpty(List* list);

u32int list_size(List* list);

#endif /* LIST_H_ */
