#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <defs.h>

typedef struct {
	int count;
} semaphore_t;

void sem_init(semaphore_t*, int initCount);

void sem_signal(semaphore_t*);

void sem_wait(semaphore_t*);

#endif
