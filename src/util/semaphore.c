#include <util/semaphore.h>
#include <asm/libasm.h>
#include <process/scheduler.h>

void sem_init(semaphore_t* sem, int initCount) {
	sem->count = initCount;
	sem->first = 0;
	sem->free = 0;
}

void sem_signal(semaphore_t* sem) {
	_cli();
    if (sem->count++ < 0) {
        scheduler_setStatus(sem->wprocess[sem->first++]->pid, READY);
        if (sem->first > MAX_WPROCESS-1) {
            sem->first = 0;
        }
    }
	_sti();


}

void sem_wait(semaphore_t* sem) {
	_cli();
    if ( --sem->count < 0 ) {
        sem->wprocess[sem->free++] = scheduler_getCurrentProcess();
        if (sem->free > MAX_WPROCESS-1) {
            sem->free = 0;
        }
        scheduler_blockCurrent(W_SEM);
    }
    _sti();
}

int sem_value(semaphore_t* sem) {
    return sem->count;
}
