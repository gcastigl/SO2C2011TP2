#include <process/process.h>
#include <session.h>
#include <util/logger.h>

extern int loadStackFrame();
int getNextPID();
PRIVATE int nextPID = 0;

// POSSIBLE FIXME: too many arguments!
void process_initialize(PROCESS* newProcess, char* name, int(*processFunc)(int, char**),
		int argc, char** argv, int stacklength, void(*cleaner)(void),
		int tty, int groundness, int status, int priority, int parentPID) {
    for (int i = 0; i < argc; i++) {
    	int len = strlen(argv[i]) + 1;
    	log(L_DEBUG, "argument %d has %d bytes", i, len);
        newProcess->argv[i] = (char*) kmalloc(len);
        memcpy(newProcess->argv[i], argv[i], len);
    }
    newProcess->argc = argc;
    memcpy(newProcess->name, name, strlen(name) + 1);
    newProcess->ownerUid = session_getEuid();
    newProcess->pid = getNextPID();
    newProcess->stacksize = stacklength;
    newProcess->stack = (int) kmalloc(stacklength);
    newProcess->ESP = loadStackFrame(processFunc, newProcess->stack + stacklength - 1, argc, newProcess->argv, cleaner);
    newProcess->tty = tty;
    newProcess->lastCalled = 0;
    newProcess->priority = priority;
    newProcess->groundness = groundness;
    newProcess->parent = parentPID;			// POSSIBLE FIXME HERE...
    newProcess->status = status;
    newProcess->waitingFlags = -1;
    for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
    	newProcess->fd_table[i].mask = 0;
    }
	log(L_DEBUG, "Creating new process: %s - PID: %d. Parent: %d\n", newProcess->name, newProcess->pid, newProcess->parent);
}

void process_finalize(PROCESS* newProcess) {
    for (int i = 0; i < newProcess->argc; i++) {
        kfree(newProcess->argv[i]);
    }
    kfree((void*) (newProcess->stack));
    kfree(newProcess);
}

int setPriority(int pid, int newPriority) {
    PROCESS *p = scheduler_getProcess(pid);
    if (p == NULL) {
    	log(L_ERROR, "could not set priority %d to pid %d", newPriority, pid);
    	return -1;
    }
    if (p->pid == 0 && newPriority == 0) {
        log(L_ERROR, "trying to remove priority from idle process! (Not allowed)");
        return -1;
    }
    p->priority = newPriority;
    return 0;
}

int getNextPID() {
    return nextPID++;
}

u32int yield() {
    _SysCall(SYSTEM_YIELD);
    return 0;
}
