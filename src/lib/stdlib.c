#include "../../include/stdlib.h"

static int nextfree = 0x300000;

void* malloc(size_t size) {
	void* temp = (void*) nextfree;
	nextfree += size;
	return temp;
}

// Malloc inicializado en 0
void* calloc(size_t size) {
	char* temp;
	int i;
	temp = (char*) malloc(size);
	for(i = 0;i < size; i++)
		temp[i] = 0;
	return (void*) temp;
}

void free(void * pointer) {
	// TODO: implementame!
}


void memcpy(void* to, void* from, size_t count) {
	size_t i;
	char* cfrom = (char *) from;
	char* cto = (char *) to;
	for (i = 0; i < count; i++) {
		cto[i] = cfrom[i];
	}
}

