#include <util/roundRobin.h>

#define MAX_RECICLED_NODES	15

PRIVATE node_t* _getRecicled();
PRIVATE void _recicle(node_t* node);

PRIVATE boolean classInitialzed = false;
PRIVATE node_t *recicledNodes[MAX_RECICLED_NODES];

void roundRobin_init(RoundRobin* list, char* name) {
	strcpy(list->name, name);
	list->prevCurrent = NULL;
	list->size = 0;
	list->removed = false;
	list->busy = false;
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
		node->next = current;
		list->prevCurrent->next = node;
		list->prevCurrent = node;
	}
	list->size++;
	list->removed = false;
}

void* roundRobin_removeCurrent(RoundRobin* list) {
	void* removed;
	if (roundRobin_isEmpty(list) || list->removed) {
		log(L_DEBUG, "[%s] not removing...", list->name);
		return NULL;
	}
	if (roundRobin_size(list) == 1) {
		removed = list->prevCurrent;
		_recicle(list->prevCurrent);
		list->prevCurrent = NULL;
	} else {
		node_t* current = list->prevCurrent->next;
		removed = current->element;
		list->prevCurrent->next = current->next;
		_recicle(current);
	}
	list->removed = true;
	list->size--;
	return removed;
}

void* roundRobin_getNext(RoundRobin* list) {
	if (roundRobin_isEmpty(list)) {
		log(L_DEBUG, "list is empty...");
		return NULL;
	}
	node_t* current = list->prevCurrent->next;
	if (list->removed) {
		list->removed = false;
		return current->element;
	}
	list->prevCurrent = current;
	list->removed = false;
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
//			log(L_DEBUG, "Saving to recicle %d", *((int*)node->element));
		}
	}
	if (!recicled) {
		free(node);
	}
}

PRIVATE node_t* _getRecicled() {
	for(int i = 0; i < MAX_RECICLED_NODES; ++i) {
		if (recicledNodes[i] != NULL) {
			node_t* node = recicledNodes[i];
			recicledNodes[i] = NULL;
			// log(L_DEBUG, "Node recicled!");
			return node;
		}
	}
	return malloc(sizeof(node_t));
}

int roundRobin_toArray(RoundRobin* list, void* array) {
	if (roundRobin_isEmpty(list)) {
		return 0;
	}
	char* charArray = (char*) array;
	int i;
	node_t* node = list->prevCurrent;
	for (i = 0; i < roundRobin_size(list); ++i) {
		memcpy(charArray, &node->element, sizeof(void*));
		charArray += sizeof(void*);
		node = node->next;
	}
	return i;
}
