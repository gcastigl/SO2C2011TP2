#include <util/roundRobin.h>

#define MAX_RECICLED_NODES	20

PRIVATE node_t* _getRecicled();
PRIVATE void _recicle(node_t* node);

PRIVATE boolean classInitialzed = false;
PRIVATE node_t *recicledNodes[MAX_RECICLED_NODES];

void roundRobin_init(RoundRobin* list) {
	list->prevCurrent = NULL;
	list->size = 0;
	// FIXME: this could be in separate initializer class method
	if (!classInitialzed) {
		for(int i = 0; i < MAX_RECICLED_NODES; ++i) {
			recicledNodes[i] = NULL;
		}
		classInitialzed = true;
	}
}

void roundRobin_add(RoundRobin* list, void* elem) {
	node_t* node = _getRecicled();
	node->element = elem;
	if (roundRobin_isEmpty(list)) {	// list is empty
		list->prevCurrent = node;
		node->next = node;
	} else if (list->size == 1) {
		node->next = list->prevCurrent;
		list->prevCurrent->next = node;
		list->prevCurrent = node;
	} else {
		node_t* current = list->prevCurrent->next;
		node->next = current->next;
		current->next = node;
	}
	list->size++;
}

void roundRobin_remove(RoundRobin* list) {
	if (roundRobin_isEmpty(list)) {
		return;
	}
	if (roundRobin_size(list) == 1) {
		_recicle(list->prevCurrent);
		list->prevCurrent = NULL;
	} else {
		node_t* current = list->prevCurrent->next;
		list->prevCurrent->next = current->next;
		_recicle(current);
	}
	list->size--;
}

void* roundRobin_getNext(RoundRobin* list) {
	if (roundRobin_isEmpty(list)) {
		return NULL;
	}
	node_t* current = list->prevCurrent->next;
	list->prevCurrent = current;
	return current->next->element;
}

boolean roundRobin_isEmpty(RoundRobin* list) {
	return list->size == 0;
}

u32int roundRobin_size(RoundRobin* list) {
	return list->size;
}


PRIVATE void _recicle(node_t* node) {
	boolean recicled = false;
	for(int i = 0; i < MAX_RECICLED_NODES && !recicled; ++i) {
		if (recicledNodes[i] == NULL) {
			recicledNodes[i] = node;
			recicled = true;
		}
	}
	if (!recicled) {
		free(node);
	}
}

PRIVATE node_t* _getRecicled() {
	for(int i = 0; i < MAX_RECICLED_NODES; ++i) {
		if (recicledNodes[i] != NULL) {
			return recicledNodes[i];
		}
	}
	return malloc(sizeof(node_t));
}
