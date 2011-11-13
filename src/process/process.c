#include <process/process.h>
#include <session.h>

extern int loadStackFrame();
int getNextPID();
PRIVATE boolean _moveProcess(int pid, RoundRobin* from, RoundRobin* to, int newState);

PRIVATE int currentPID = 0;
PRIVATE int nextPID = 0;

PRIVATE RoundRobin activeProcess;
PRIVATE RoundRobin blockedProcess;

void process_initialize() {
	roundRobin_init(&activeProcess);
	roundRobin_init(&blockedProcess);
}

void createProcess(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, void(*cleaner)(void), int tty,
        int groundness, int status, int priority) {
    if (roundRobin_size(&activeProcess) + roundRobin_size(&blockedProcess) >= MAX_PROCESSES) {
    	log(L_ERROR, "Could not create proces %s, max process running", name);
        return;
    }
    PROCESS *newProcess;
    newProcess = kmalloc(sizeof(PROCESS));
    void *stack = kmalloc(stacklength);
    for (int i = 0; i < argc; i++) {
        newProcess->argv[i] = (char*) kmalloc(strlen(argv[i])+1);
        strcpy(newProcess->argv[i], argv[i]);
    }
    newProcess->argc = argc;
    memcpy(newProcess->name, name, strlen(name) + 1);
    newProcess->ownerUid = session_getEuid();
    newProcess->pid = getNextPID();
    newProcess->stacksize = stacklength;
    newProcess->stackstart = (int) stack + stacklength - 1;
    newProcess->ESP = loadStackFrame(processFunc, newProcess->stackstart, argc, newProcess->argv, cleaner);
    newProcess->tty = tty;
    newProcess->lastCalled = 0;
    newProcess->priority = priority;
    newProcess->groundness = groundness;
    newProcess->parent = currentPID;			// POSSIBLE FIXME HERE...
    newProcess->status = status;
		log(L_DEBUG, "Creating new process: %s - PID: %d\n", name, newProcess->pid);
    for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
    	newProcess->fd_table[i].mask = 0;
    }
    if (status == BLOCKED) {
    	roundRobin_add(&blockedProcess, newProcess);
    } else {
    	roundRobin_add(&activeProcess, newProcess);
    }
    if (groundness == FOREGROUND) {
        PROCESS *p = getProcessByPID(currentPID);
        if (p != NULL) {
        	p->status = BLOCKED;
            p->lastCalled = 0;
            switchProcess();
        }
    }
}

int getCurrentPID() {
    return currentPID;
}

void setCurrentPID(int pid) {
	log(L_DEBUG, "setting current process as %d", pid);
	boolean pidExists = false;
	for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
		PROCESS* p = roundRobin_getNext(&activeProcess);
		if (p->pid == pid) {
			pidExists = true;
		}
	}
	if (pidExists) {
		currentPID = pid;
	} else {
		log(L_ERROR, "Trying to set %d as active process, but does not exists as an active process!", pid);
	}
}

PROCESS *getCurrentProcess(void) {
    return getProcessByPID(currentPID);
}

//TODO: fixear parte comentada
void killChildren(int pid) {
	log(L_DEBUG, "killing child process PID: %d", pid);
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        /*if (process[i].slotStatus == OCCUPIED) {
            if (process[i].parent == pid) {
                kill(process[i].pid);
            }
        }*/
    }
}

//TODO: fixear parte comentada
void kill(int pid) {
    int i;
    PROCESS *p = NULL;
    PROCESS *parent;
    log(L_DEBUG, "killing process PID: %d", pid);
    if (pid < MAX_TTYs) {
        return;
    }
    for (i = 0; i < MAX_PROCESSES; i++) {
        /*if (process[i].slotStatus == OCCUPIED) {
            if (process[i].pid == pid) {
                if (permission_user_isOwner(process[i].ownerUid)) {
                    p = &process[i];
                    break;
                } else {
                    printf("Access denied\n");
                    errno = EACCES;
                    return;
                }
            }
        }*/
    }
    if (p == NULL) {
        printf("PID is not valid\n");
        return;
    }
    killChildren(pid);
    parent = getProcessByPID(p->parent);
    if (parent) {
        if (parent->status == BLOCKED) {
            parent->status = READY;
        }
    }
    //Free process memory
}

void setPriority(int pid, int newPriority) {
    PROCESS *p = getProcessByPID(pid);
    if (p == NULL) {
    	log(L_ERROR, "could not set priority %d to pid %d", newPriority, pid);
    	return;
    }
    p->priority = newPriority;
}

void clean() {
	_cli();
	log(L_DEBUG, "EXECUTING CLEAN! - removing %d", currentPID);
    PROCESS *temp;
    temp = getProcessByPID(currentPID);
    while((temp = roundRobin_getNext(&activeProcess))->pid != currentPID) {
//    	log(L_DEBUG, "next... %d", temp->pid); // Scroll until list is at currentPID
    }
	roundRobin_removeCurrent(&activeProcess);
	temp = getProcessByPID(temp->parent);
	log(L_DEBUG, "Removed from active processes. Parent: %d - %d", temp->parent, temp);
    if (temp != NULL) {
    	process_setStatus(temp->parent, READY);
        if (temp->status == BLOCKED) {
            temp->status = READY;
        }
    }
    kfree((void*) temp->stackstart - temp->stacksize + 1);
    for (int i = 0; i < temp->argc; i++) {
        kfree((void*) temp->argv[i]);
    }
    switchProcess();
}

int getNextPID() {
    return nextPID++;
}

PROCESS *getProcessByPID(int pid) {
	PROCESS* current;
    // Search active processes
	for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
    	current = roundRobin_getNext(&activeProcess);
		if (current->pid == pid) {
//			log(L_DEBUG, "process %d was found as activeProcess - %d", pid, current);
			return current;
		}
    }
    // Search blocked processes
    for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
		current = roundRobin_getNext(&blockedProcess);
		if (current->pid == pid) {
//			log(L_DEBUG, "process %d was found as blockedProcess", pid);
			return current;
		}
    }
	log(L_ERROR, "process %d was NOT found", pid);
    return NULL;
}

void* getActiveProcess() {
	return &activeProcess;
}

void* getBlockedProcess() {
	return &blockedProcess;
}

boolean process_setStatus(u32int pid, u32int status) {
	log(L_DEBUG, "changing process %d to state %d", pid, status);
	if (status == BLOCKED) {
		return _moveProcess(pid, &activeProcess, &blockedProcess, status);
	} else if (status == READY || status == RUNNING) {
		return _moveProcess(pid, &blockedProcess, &activeProcess, status);
	}
	return false;
}

PRIVATE boolean _moveProcess(int pid, RoundRobin* from, RoundRobin* to, int newState) {
	for (int i = 0; i < roundRobin_size(from); i++) {
		PROCESS* p = roundRobin_getNext(from);
		if (p->pid == pid) {
			p->status = newState;
			roundRobin_add(to, roundRobin_removeCurrent(from));
			return true;
		}
	}
	log(L_DEBUG, "could not move process %d", pid);
	return false;
}
