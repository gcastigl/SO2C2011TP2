#include <process/process.h>
#include <command.h>

PROCESS* _nextTask(int withPriority);
PRIVATE void saveESP(int oldESP);

PRIVATE int firstTime = true;
PRIVATE int schedulerActive = false;
PRIVATE int usePriority;
PRIVATE int count100;

void scheduler_init(int withPriority) {
    count100 = 0;
    usePriority = withPriority;
    process_create("Idle", &idle_cmd, 0, NULL, DEFAULT_STACK_SIZE, &clean, -1, BACKGROUND, READY, VERY_LOW);
    schedulerActive = true;
}

int getNextProcess(int oldESP) {
    PROCESS *proc, *proc2;
    proc2 = process_getPID(process_currentPID());
    if (proc2->status == RUNNING) {
        proc2->status = READY;
    }
    proc = _nextTask(usePriority);
    proc->status = RUNNING;
    proc->lastCalled = 0;
    if (!firstTime) {
        saveESP(oldESP); // el oldESP esta el stack pointer del proceso
    } else {
        firstTime = false;
    }
    process_setCurrent(proc->pid);
    setFD(proc->tty);
    return proc->ESP;
}

/* getNextTask
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Devuelve el próximo proceso a ejecutar
**/
PROCESS* _nextTask(int withPriority) {
    int i;
    PROCESS* nextReady = NULL;
    int temp, bestScore = 0;
    PROCESS *current;
    RoundRobin* active =  (RoundRobin*) process_getActive();
    for (i = 0; i < roundRobin_size(active); i++) {
    	current = roundRobin_getNext(active);
		current->lastCalled++;
		if (withPriority) {
			temp = current->priority * P_RATIO + current->lastCalled;
		} else {
			temp = current->lastCalled;
		}
		if (current->priority == PNONE) {
			temp /= 5;
		}
		if (temp > bestScore) {
			bestScore = temp;
			nextReady = current;
		}
    }
    // log(L_DEBUG, "returning: %s", nextReady->name);
    last100[(count100 = (count100 + 1) % 100)] = nextReady->pid;
    return nextReady;
    //notar que si no hay procesos disponibles, retornara &processes[0], o sea idle

}

void scheduler_setActive(boolean active) {
	schedulerActive = active;
}

boolean scheduler_isActive() {
	return schedulerActive;
}

/* saveESP
 * Recibe como parametros:
 * - valor del viejo ESP
 *
 * Guarda el ESP del proceso actual
 */
PRIVATE void saveESP(int oldESP) {
    PROCESS *proc = process_getCurrent();
    if (proc != NULL) {
        proc->ESP = oldESP;
    } else {
    	// FIXME: The code normally reaches this else when a process has just returned and
    	// because it was the current process it does not exist any more.
    	log(L_ERROR, "current process is NULL!!");
    }
}

