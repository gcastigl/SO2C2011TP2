#include <util/list.h>
#include <lib/string.h>
#include <lib/stdlib.h>

#include <lib/stdio.h>

void list_init(List* list, char* name) {
	strcpy(list->name, name);
	list->size = 0;
	list->first = NULL;
}

void list_add(List* list, void* elem) {
	node_t* node = malloc(sizeof(node_t));
	node->element = elem;
	if (list_isEmpty(list)) {
		node->next = NULL;
		list->first = node;
	} else {
		node->next = list->first;
		list->first = node;
	}
	list->size++;
}

void* list_remove(List* list, int index) {
	if (list_isEmpty(list) || index >= list_size(list)) {
		return NULL;
	}
	void* removed;
	node_t* curr = list->first;
	if (index == 0) {
		list->first = curr->next;
	} else {
		node_t* prev = NULL;
		for (int i = 0; i < index; ++i) {
			curr = curr->next;
			prev = curr;
		}
		prev->next = curr->next;
	}
	removed = curr->element;
	free(curr);
	list->size--;
	return removed;
}

boolean list_removeElem(List* list, void* elem, int (cmp)(void*, void*)) {
	if (list_isEmpty(list)) {
		return false;
	}
	node_t* curr = list->first;
	node_t* prev = NULL;
	while (cmp(elem, curr->element) != 0 && curr != NULL) {
		curr = curr->next;
		prev = curr;
	}
	if (curr == NULL) {
		return false;
	}
	if (prev == NULL) {	// Trying to remove first node
		list->first = curr->next;
	} else {
		prev->next = curr->next;
	}
	free(curr);
	list->size--;
	return true;
}

void* list_get(List* list, int index) {
	// log(L_DEBUG, "list size: %d", list_size(list));
	if (list_isEmpty(list) || index >= list_size(list)) {
		printf("[list] returning NULL");
		return NULL;
	}
	node_t* curr = list->first;
	for (int i = 0; i < index && curr->next != NULL; ++i) {
		curr = curr->next;
	}
	// log(L_DEBUG, "%d", curr);
	return curr->element;
}

boolean list_isEmpty(List* list) {
	return list->first == NULL;
}

u32int list_size(List* list) {
	return list->size;
}
