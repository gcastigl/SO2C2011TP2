#include <process/scheduler.h>
#include <command.h>

PRIVATE PROCESS* _nextTask(int withPriority);
PRIVATE void saveESP(int oldESP);
PRIVATE void killChildren(int pid);
/*
 * Función cementerio al cual van a parar todos los procesos una vez que terminan
 */
PRIVATE void clean();


PRIVATE PROCESS* allProcess[MAX_PROCESSES];
PRIVATE PROCESS* current;

PRIVATE int schedulerActive = false;
PRIVATE int usePriority;
PRIVATE int count100;
PRIVATE int firstTime = true;

void scheduler_init(int withPriority) {
	_cli();
	// move_stack((void*)0xE0000000, 0x2000);
    count100 = 0;
    usePriority = withPriority;
    schedulerActive = true;
    for (int i = 0; i < MAX_PROCESSES; i++) {
    	allProcess[i] = NULL;
    }
    current = NULL;
	scheduler_schedule("Idle", &idle_cmd, 0, NULL, DEFAULT_STACK_SIZE, 0, BACKGROUND, READY, VERY_LOW);
	_sti();
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
    PROCESS *proc = scheduler_getCurrentProcess();
    if (proc == NULL) {			// Should never be here...
    	errno = E_ACCESS;
    	log(L_ERROR, "current process is NULL!!");
    	return;
    }
	if (proc->status != FINALIZED) {
		proc->ESP = oldESP;
	}
}

void scheduler_schedule(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, int tty, int groundness, int status, int priority) {
	// Check if max process reached
	_cli();
	int i = 0;
	while (allProcess[i] != NULL && i < MAX_PROCESSES) {
		i++;
	}
	if (i == MAX_PROCESSES) {
		log(L_ERROR, "Could not create process %s. Max processes reached!", name);
		return;
	}
    PROCESS* newProcess = (PROCESS*)kmalloc(sizeof(PROCESS));
	allProcess[i] = newProcess;
		log(L_DEBUG, "%s is now on position: %d", name, i);
    process_initialize(newProcess, name, processFunc, argc, argv, stacklength,
    		&clean, tty, groundness, status, priority, (current == NULL) ? 0 : current->pid);
    _sti();
    if (groundness == FOREGROUND) {
        if (current != NULL) {
            current->lastCalled = 0;
            scheduler_blockCurrent(W_CHILD);
        }
    }
}

int getNextProcess(int oldESP) {
    PROCESS *next = _nextTask(usePriority);
    next->status = RUNNING;
    next->lastCalled = 0;
    if (!firstTime) {
        saveESP(oldESP); 			// en el oldESP esta el stack pointer del proceso
    } else {
        firstTime = false;
    }
    scheduler_setCurrent(next);
    setFD(next->tty);				// Sets the sys write call output to the tty corresponding to the process
    return next->ESP;
}

/* getNextTask
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Devuelve el próximo proceso a ejecutar
**/
PRIVATE PROCESS* _nextTask(int withPriority) {
	// Schdule tasks...
	PROCESS *current = NULL, *nextReady;
	int bestScore = 0, temp;
	for (int i = 0; i < MAX_PROCESSES; ++i) {
		current = allProcess[i];
		if (current == NULL) {				// slot is empty...
			continue;
		}
		if (current->status == FINALIZED) {	// process is finalized, emty this slot
		    process_finalize(current);
			allProcess[i] = NULL;
			continue;
		}
		if (current->status != BLOCKED && current->priority != PNONE) {
			current->lastCalled++;
			if (withPriority == true) {
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
	}
	last100[(count100 = (count100 + 1) % 100)] = nextReady->pid;
	return nextReady;
}

void scheduler_setStatus(u32int pid, u32int status) {
	for (int i = 0; i < MAX_PROCESSES; ++i) {
		if (allProcess[i] != NULL && allProcess[i]->pid == pid) {
			allProcess[i]->status = status;
			allProcess[i]->waitingFlags= -1;
			// log(L_DEBUG, "(%s)%d is now %s", allProcess[i]->name, pid, (status == 0) ? "Blocked" : ((status == 1) ? "Ready" : "Running"));
			break;
		}
	}
}

void scheduler_blockCurrent(block_t waitFlag) {
    current->status = BLOCKED;
    current->waitingFlags = waitFlag;
    yield();
}

PRIVATE void clean() {
    	log(L_DEBUG, "finalized: %s (%d)", current->name, current->pid, current->parent);
    current->status = FINALIZED;
	scheduler_setStatus(current->parent, READY);
    switchProcess();
}

PROCESS *scheduler_getProcess(int pid) {
    // Search blocked processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
		if (allProcess[i] != NULL && allProcess[i]->pid == pid) {
			return allProcess[i];
		}
    }
	log(L_ERROR, "process %d was NOT found", pid);
    return NULL;
}

int scheduler_currentPID() {
    return current->pid;
}

void scheduler_setCurrent(PROCESS* p) {
    current = p;
}

void kill(int pid) {
    if (pid < MAX_TTYs) {
    	log(L_ERROR, "Trying to kill TTY: %d (Not Allowed)", pid);
        return;
    }
    log(L_DEBUG, "killing process PID: %d", pid);
    for (int i = 0; i < MAX_PROCESSES; i++) {
    	if (allProcess[i] != NULL && allProcess[i]->pid == pid) {
    	    killChildren(pid);
    		scheduler_setStatus(allProcess[i]->parent, READY);
    		allProcess[i]->status = FINALIZED;
		}
    }
}

void killCurrent() {
    kill(current->pid);
    switchProcess();
}

PRIVATE void killChildren(int pid) {
	log(L_DEBUG, "killing child process PID: %d", pid);
    for (int i = 0; i < MAX_PROCESSES; i++) {
    	if (allProcess[i] != NULL && allProcess[i]->parent == pid) {
    		kill(allProcess[i]->pid);
		}
    }
}

PROCESS *scheduler_getCurrentProcess() {
    return current;
}

PROCESS **scheduler_getAllProcesses() {
	return allProcess;
}

u32int scheduler_activeProcesses() {
	int active = 0;
	for (int i = 0; i < MAX_PROCESSES; ++i) {
		if (allProcess[i] != NULL && allProcess[i]->status != BLOCKED) {
			active++;
		}
	}
	return active;
}

