#include <process/process.h>
#include <session.h>
#include <util/logger.h>
#include <process/scheduler.h>
#include <lib/stdlib.h>
#include <memory/paging.h>

extern int loadStackFrame();
int getNextPID();
PRIVATE int nextPID = 0;

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

PUBLIC void _expandStack() {
    PROCESS *proc = scheduler_getCurrentProcess();
    int esp = _ESP;
    int newSize = DEFAULT_STACK_SIZE + proc->stacksize;
    void *new_stack_start = (void *)kmalloc_a(newSize);
    int offset = (int)new_stack_start - proc->stack;
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
        if ((proc->stack < tmp) && ((proc->stack + proc->stacksize) < esp))
        {
          tmp = tmp + offset;
          u32int *tmp2 = (u32int*)i;
          *tmp2 = tmp;
        }
      }

    kfree((void*)proc->stack);
    proc->stack = (int)new_stack_start;
    proc->stacksize = newSize;
    proc->ESP = proc->ESP + offset;
}

PUBLIC void process_checkStack() {
    PROCESS *proc = scheduler_getCurrentProcess();
    if (proc == NULL)
        return;
    if (proc->pid > MAX_TTYs)
        log(L_INFO, "Process %s: ESP: 0x%x stackStart: 0x%x", proc->name, proc->ESP, proc->stack);
/*    if (proc->ESP < proc->stack + 0x500) {
        registers_t regs;
        regs.esp = proc->ESP;
        panic("asdasdasd", 1, true);
    }
*/    
    if (proc->ESP < proc->stack + 0x500) {
        log(L_INFO, "Expanding stack(0x%x @ 0x%x) for %s", proc->stack, proc->name);
        _expandStack();
        log(L_INFO, "EXPANDED stack(0x%x) for %s", proc->stack, proc->name);
    }
}
