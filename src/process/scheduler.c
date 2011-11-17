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

PRIVATE int currentPID = 0;
PRIVATE int schedulerActive = false;
PRIVATE int usePriority;
PRIVATE int count100;
PRIVATE int firstTime = true;

PRIVATE int lastProcessIndex;

void scheduler_init(int withPriority) {
    count100 = 0;
    usePriority = withPriority;
    schedulerActive = true;
    for (int i = 0; i < MAX_PROCESSES; i++) {
    	allProcess[i] = NULL;
    }
    lastProcessIndex = 0;
	scheduler_schedule("Idle", &idle_cmd, 0, NULL, DEFAULT_STACK_SIZE, 0, BACKGROUND, READY, VERY_LOW);
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
    if (proc != NULL) {
        proc->ESP = oldESP;
    } else {
    	// FIXME: The code normally reaches this else when a process has just returned and
    	// because it was the current process it does not exist any more.
    	log(L_ERROR, "current process is NULL!!");
    }
}

void scheduler_schedule(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, int tty, int groundness, int status, int priority) {
	// Check if max process reached
	// TODO: check max number of process active....

    PROCESS* newProcess = kmalloc(sizeof(PROCESS));
    process_initialize(newProcess, name, processFunc, argc, argv, stacklength,
    		&clean, tty, groundness, status, priority, currentPID);
    for (int i = 0; i < MAX_PROCESSES; i++) {
    	if (allProcess[i] == NULL) {
    		allProcess[i] = newProcess;
    		log(L_DEBUG, "%s is now on position: %d", name, i);
    		break;
    	}
    }
    if (groundness == FOREGROUND) {
        PROCESS *p = scheduler_getProcess(currentPID);
        if (p != NULL) {
        	scheduler_setStatus(currentPID, BLOCKED);
        	p->status = BLOCKED;
            p->lastCalled = 0;
            switchProcess();
        }
    }
}

int getNextProcess(int oldESP) {
    PROCESS *proc, *proc2;
    proc2 = scheduler_getProcess(scheduler_currentPID());
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
    scheduler_setCurrent(proc->pid);
    setFD(proc->tty);				// Sets the output to the tty corresponding to the process
    return proc->ESP;
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
		if (current->status != BLOCKED && current->status != PNONE) {
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

boolean scheduler_setStatus(u32int pid, u32int status) {
	for (int i = 0; i < MAX_PROCESSES; ++i) {
		if (allProcess[i] != NULL && allProcess[i]->pid == pid) {
			allProcess[i]->status = status;
			log(L_DEBUG, "pid %d now has status %d", pid, status);
			return true;
		}
	}
	return false;
}

PRIVATE void clean() {
    PROCESS *temp = scheduler_getProcess(currentPID);
    	log(L_DEBUG, "CLEAN! - name: %s pid: %d / parent: %d", temp->name, temp->pid, temp->parent);
    for (int i = 0; i < MAX_PROCESSES; ++i) {
		if (allProcess[i] != NULL && allProcess[i]->pid == currentPID) {
			allProcess[i] = NULL;
		}
	}
    kfree((void*) temp->stackstart - temp->stacksize + 1);
    for (int i = 0; i < temp->argc; i++) {
        kfree((void*) temp->argv[i]);
    }
	scheduler_setStatus(temp->parent, READY);
    switchProcess();
}

PROCESS *scheduler_getProcess(int pid) {
    // Search blocked processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
		if (allProcess[i] != NULL && allProcess[i]->pid == pid) {
//			log(L_DEBUG, "process %d was found at pos %d", pid, i);
			return allProcess[i];
		}
    }
	log(L_ERROR, "process %d was NOT found", pid);
    return NULL;
}

int scheduler_currentPID() {
    return currentPID;
}

void scheduler_setCurrent(int pid) {
	currentPID = pid;
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
    		process_finalize(allProcess[i]);
    		allProcess[i] = NULL;
		}
    }
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
    return scheduler_getProcess(currentPID);
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

