#include <process/process.h>
#include <session.h>
#include <util/logger.h>
#include <process/scheduler.h>
#include <lib/stdlib.h>

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
    newProcess->stack = (int) kmalloc_a(stacklength);
    log(L_DEBUG, "%s starts at 0x%x to 0x%x", name, newProcess->stack, newProcess->stack + stacklength - 1); 
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

PRIVATE void _expandStack() {
    _cli();
    PROCESS *proc = scheduler_getCurrentProcess();
    int esp = _ESP;
    int offset = DEFAULT_STACK_SIZE;
    int newSize = offset + proc->stacksize;
    void *new_stack_start = (void *)kmalloc_a(newSize);

    void *old_stack_start = (void*)proc->stack;

    memcpy(new_stack_start, old_stack_start, proc->stacksize);
    
      // Backtrace through the original stack, copying new values into
      // the new stack.
      for(int i = (u32int)new_stack_start; i > (u32int)new_stack_start-newSize; i -= 4)
      {
        u32int tmp = * (u32int*)i;
        // If the value of tmp is inside the range of the old stack, assume it is a base pointer
        // and remap it. This will unfortunately remap ANY value in this range, whether they are
        // base pointers or not.
        if (( esp < tmp) && (tmp < esp))
        {
          tmp = tmp + offset;
          u32int *tmp2 = (u32int*)i;
          *tmp2 = tmp;
        }
      }

    kfree(old_stack_start);
    proc->stack = (int)new_stack_start;
    proc->stacksize = newSize;
    proc->ESP = esp + offset;
    _sti();
}

PUBLIC void process_updateStack() {
    PROCESS *proc = scheduler_getCurrentProcess();
    if (proc == NULL)
        return;
    if ((proc->stack - proc->ESP) > (proc->stacksize / 2)) {
        log(L_INFO, "Expanding stack(0x%x) for %s", proc->stack, proc->name);
        _expandStack();
        log(L_INFO, "EXPANDED stack(0x%x) for %s", proc->stack, proc->name);
    }
}
