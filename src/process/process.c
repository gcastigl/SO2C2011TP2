#include <process/process.h>
#include <session.h>

PRIVATE void killChildren(int pid);
extern int loadStackFrame();
int getNextPID();
PRIVATE boolean _moveProcess(int pid, RoundRobin* from, RoundRobin* to, int newState);

PRIVATE int currentPID = 0;
PRIVATE int nextPID = 0;

PRIVATE RoundRobin activeProcess;
PRIVATE RoundRobin blockedProcess;

void process_initialize() {
	roundRobin_init(&activeProcess, "Active");
	roundRobin_init(&blockedProcess, "Blocked");
}

void process_create(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, void(*cleaner)(void), int tty,
        int groundness, int status, int priority) {
    if (roundRobin_size(&activeProcess) + roundRobin_size(&blockedProcess) >= MAX_PROCESSES) {
    	log(L_ERROR, "Could not create proces %s, max process running", name);
        return;
    }
    PROCESS *newProcess = kmalloc(sizeof(PROCESS));
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
    for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
    	newProcess->fd_table[i].mask = 0;
    }
	log(L_DEBUG, "Creating new process: %s - PID: %d. Parent: %d\n", newProcess->name, newProcess->pid, newProcess->parent);
    if (status == BLOCKED) {
    	roundRobin_add(&blockedProcess, newProcess);
    } else {
    	roundRobin_add(&activeProcess, newProcess);
    }
    if (groundness == FOREGROUND) {
        PROCESS *p = process_getPID(currentPID);
        if (p != NULL) {
        	process_setStatus(currentPID, BLOCKED);
        	p->status = BLOCKED;
            p->lastCalled = 0;
            switchProcess();
        }
    }
}

int process_currentPID() {
    return currentPID;
}

void process_setCurrent(int pid) {
	// log(L_DEBUG, "setting current process as %d", pid);
	for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
		PROCESS* p = roundRobin_getNext(&activeProcess);
		if (p->pid == pid) {
			currentPID = pid;
			return;
		}
	}
	log(L_ERROR, "Trying to set %d as active process, but does not exists as an active process!", pid);
}

PROCESS *process_getCurrent() {
    return process_getPID(currentPID);
}

void process_kill(int pid) {
    if (pid < MAX_TTYs) {
    	log(L_ERROR, "Trying to kill TTY: %d (Not Allowed)", pid);
        return;
    }
    log(L_DEBUG, "killing process PID: %d", pid);
    for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
    	PROCESS *p = roundRobin_getNext(&activeProcess);
    	if (p->pid == pid) {
    		roundRobin_removeCurrent(&activeProcess);
    	    killChildren(pid);
    		process_setStatus(p->parent, READY);
    	    //TODO: Free process memory
    		return;
		}
    }
}

PRIVATE void killChildren(int pid) {
	log(L_DEBUG, "killing child process PID: %d", pid);
    for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
		PROCESS* p = roundRobin_getNext(&activeProcess);
    	if (p->parent == pid) {
			process_kill(p->pid);
    	}
    }
    for (int i = 0; i < roundRobin_size(&blockedProcess); i++) {
		PROCESS* p = roundRobin_getNext(&blockedProcess);
		if (p->parent == pid) {
			process_kill(p->pid);
		}
	}
}

void setPriority(int pid, int newPriority) {
    PROCESS *p = process_getPID(pid);
    if (p == NULL) {
    	log(L_ERROR, "could not set priority %d to pid %d", newPriority, pid);
    	return;
    }
    p->priority = newPriority;
}

void clean() {
	_cli();
    PROCESS *temp = process_getPID(currentPID);
    	log(L_DEBUG, "CLEAN! - name: %s pid: %d / parent: %d", temp->name, temp->pid, temp->parent);
    while((temp = roundRobin_getNext(&activeProcess))->pid != currentPID) {
//    	log(L_DEBUG, "next... %d", temp->pid); // Scroll until list is at currentPID
    }
	roundRobin_removeCurrent(&activeProcess);
    kfree((void*) temp->stackstart - temp->stacksize + 1);
    for (int i = 0; i < temp->argc; i++) {
        kfree((void*) temp->argv[i]);
    }
	process_setStatus(temp->parent, READY);
	// setCurrentPID(temp->parent);	// FIXME: this line should not be commented but makes te SO to fail
    switchProcess();
}

int getNextPID() {
    return nextPID++;
}

PROCESS *process_getPID(int pid) {
	PROCESS* current;
    // Search active processes
	for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
    	current = roundRobin_getNext(&activeProcess);
		if (current->pid == pid) {
			// log(L_DEBUG, "process %d was found as activeProcess - %d", pid, current);
			return current;
		}
    }
    // Search blocked processes
    for (int i = 0; i < roundRobin_size(&activeProcess); i++) {
		current = roundRobin_getNext(&blockedProcess);
		if (current->pid == pid) {
			// log(L_DEBUG, "process %d was found as blockedProcess", pid);
			return current;
		}
    }
	log(L_ERROR, "process %d was NOT found", pid);
    return NULL;
}

void* process_getActive() {
	return &activeProcess;
}

void* process_getBlocked() {
	return &blockedProcess;
}

boolean process_setStatus(u32int pid, u32int status) {
	if (status == BLOCKED) {
		log(L_DEBUG, "moving %d [%s] -> [%s]", pid, activeProcess.name, blockedProcess.name);
		return _moveProcess(pid, &activeProcess, &blockedProcess, status);
	} else if (status == READY || status == RUNNING) {
		log(L_DEBUG, "moving %d [%s] -> [%s]", pid, blockedProcess.name, activeProcess.name);
		return _moveProcess(pid, &blockedProcess, &activeProcess, status);
	}
	return false;
}

PRIVATE boolean _moveProcess(int pid, RoundRobin* from, RoundRobin* to, int newState) {
	for (int i = 0; i < roundRobin_size(from); i++) {
		PROCESS* p = roundRobin_getNext(from);
		if (p->pid == pid) {
			roundRobin_removeCurrent(from);
			p->status = newState;
			roundRobin_add(to, p);
			log(L_DEBUG, "process %d has been set to: %d", pid, newState);
			return true;
		}
	}
	log(L_DEBUG, "could not move process %d", pid);
	return false;
}
