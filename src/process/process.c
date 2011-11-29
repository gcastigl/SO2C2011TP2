#include <process/process.h>
#include <session.h>
#include <util/logger.h>
#include <process/scheduler.h>
#include <lib/stdlib.h>
#include <memory/paging.h>

extern page_directory_t *current_directory;

extern int loadStackFrame();
int getNextPID();
PRIVATE int nextPID = 0;

void process_initialize(PROCESS* newProcess, char* name, int(*processFunc)(int, char**),
		int argc, char** argv, int stacklength, void(*cleaner)(void),
		int tty, int groundness, int status, int priority, int parentPID) {
    for (int i = 0; i < argc; i++) {
    	int len = strlen(argv[i]) + 1;
        newProcess->argv[i] = (char*) kmalloc(len);
        memcpy(newProcess->argv[i], argv[i], len);
    }
    newProcess->argc = argc;
    memcpy(newProcess->name, name, strlen(name) + 1);
    newProcess->ownerUid = session_getEuid();
    newProcess->pid = getNextPID();
    newProcess->stacksize = stacklength;
    newProcess->stack = paging_reserveStack(stacklength);
    log(L_DEBUG, "%s starts at 0x%x to 0x%x", name, newProcess->stack, newProcess->stack + stacklength - 1);
    newProcess->ESP = loadStackFrame(processFunc, newProcess->stack + stacklength - 1, argc, newProcess->argv, cleaner);
    newProcess->tty = tty;
    newProcess->lastCalled = 0;
    newProcess->priority = priority;
    newProcess->groundness = groundness;
    newProcess->parent = parentPID;
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
    paging_dropStack(newProcess->stack, newProcess->stacksize);
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

PUBLIC void _expandStack() {
    _cli();
    PROCESS *proc = scheduler_getCurrentProcess();
    proc->stack -= PAGE_SIZE;
    proc->stacksize += PAGE_SIZE;
    // FIXME: check for used pages.
    get_page(proc->stack, true, current_directory);
    log(L_DEBUG, "EXPANDED: %s starts at 0x%x to 0x%x", proc->name, proc->stack, proc->stack + proc->stacksize - 1);
    printf("EXPANDED: %s starts at 0x%x to 0x%x (esp: 0x%x)\n", proc->name, proc->stack, proc->stack + proc->stacksize - 1, proc->ESP);
}

PUBLIC void process_checkStack() {
    PROCESS *proc = scheduler_getCurrentProcess();
    if (proc == NULL)
        return;
    if (proc->pid > MAX_TTYs)
        log(L_INFO, "Process %s: ESP: 0x%x stackStart: 0x%x", proc->name, proc->ESP, proc->stack);
  
    if (proc->ESP < proc->stack + 0x750) {
        log(L_INFO, "Expanding stack(0x%x @ 0x%x) for %s", proc->stack, proc->ESP, proc->name);
        _expandStack();
        log(L_INFO, "EXPANDED stack(0x%x) for %s", proc->stack, proc->name);
    }
}
