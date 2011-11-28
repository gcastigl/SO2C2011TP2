#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <defs.h>
#include <process/process.h>


#define MAX_WPROCESS    5


typedef struct {
	int count;
    PROCESS* wprocess[MAX_WPROCESS];
    u32int first, free;
} semaphore_t;

void sem_init(semaphore_t*, int initCount);

void sem_signal(semaphore_t*);

void sem_wait(semaphore_t*);

int sem_value(semaphore_t* sem);

#endif
