#ifndef ROUNDROBIN_H_
#define ROUNDROBIN_H_

#include <defs.h>
#include <lib/stdlib.h>

struct node {
	void* element;
	struct node* next;
};

typedef struct node node_t;

typedef struct {
	node_t* prevCurrent;
	int size;
	boolean removed;
} RoundRobin;


void roundRobin_init(RoundRobin* list);

void roundRobin_add(RoundRobin* list, void* elem);

void* roundRobin_removeCurrent(RoundRobin* list);

void* roundRobin_getNext(RoundRobin* list);

boolean roundRobin_isEmpty(RoundRobin* list);

u32int roundRobin_size(RoundRobin* list);

#endif
