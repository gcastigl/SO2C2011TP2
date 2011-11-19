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
    newProcess->parent = parentPID;			// POSSIBLE FIXME HERE...
    newProcess->status = status;
    for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
    	newProcess->fd_table[i].mask = 0;
    }
	log(L_DEBUG, "Creating new process: %s - PID: %d. Parent: %d\n", newProcess->name, newProcess->pid, newProcess->parent);
}

void process_finalize(PROCESS* newProcess) {
    for (int i = 0; i < newProcess->argc; i++) {
        kfree(newProcess->argv[i]);
    }
    kfree((void*) (newProcess->stackstart - newProcess->stacksize - 1));
    kfree(newProcess);
}

void setPriority(int pid, int newPriority) {
    PROCESS *p = scheduler_getProcess(pid);
    if (p == NULL) {
    	log(L_ERROR, "could not set priority %d to pid %d", newPriority, pid);
    	return;
    }
    p->priority = newPriority;
}

int getNextPID() {
    return nextPID++;
}

u32int yield() {
    _SysCall(SYSTEM_YIELD);
    return 0;
}
