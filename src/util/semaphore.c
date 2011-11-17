#include <util/semaphore.h>

//FIXME: THIS IS ONLY A MOCK UP SEMAPHORE!!

void sem_init(semaphore_t* sem, int initCount) {
	sem->count = initCount;
}

void sem_signal(semaphore_t* sem) {
	sem->count++;
}

void sem_wait(semaphore_t* sem) {
	sem->count--;
	while (sem->count < 0) {
		// WAIT....
	}
}
