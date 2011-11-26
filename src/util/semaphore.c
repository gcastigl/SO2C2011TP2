#include <util/semaphore.h>
#include <asm/libasm.h>

//FIXME: THIS IS ONLY A MOCK UP SEMAPHORE!!

void sem_init(semaphore_t* sem, int initCount) {
	sem->count = initCount;
}

void sem_signal(semaphore_t* sem) {
	_cli();
	sem->count++;
	_sti();
}

void sem_wait(semaphore_t* sem) {
	_cli();
	sem->count--;
	_sti();
	while (sem->count < 0) {
		// WAIT....
	}
}
