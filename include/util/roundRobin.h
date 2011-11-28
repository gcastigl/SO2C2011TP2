#ifndef ROUNDROBIN_H_
#define ROUNDROBIN_H_

#include <defs.h>
#include <lib/stdlib.h>
#include <lib/string.h>

struct node {
	void* element;
	struct node* next;
};

typedef struct node node_t;

typedef struct {
	char name[24];
	node_t* prevCurrent;
	int size;
	boolean removed;
	boolean busy;
} RoundRobin;


void roundRobin_init(RoundRobin* list, char* name);

void roundRobin_add(RoundRobin* list, void* elem);

void* roundRobin_removeCurrent(RoundRobin* list);

void* roundRobin_getNext(RoundRobin* list);

boolean roundRobin_isEmpty(RoundRobin* list);

u32int roundRobin_size(RoundRobin* list);

int roundRobin_toArray(RoundRobin* list, void* array);

#endif
