#include <process/scheduler.h>
#include <command.h>
#include <memory/paging.h>
#include <memory/frame.h>

PRIVATE PROCESS* _nextTask(int withPriority);
PRIVATE void saveESP(int oldESP);
PRIVATE void killChildren(int pid);

void downPages( PROCESS *p );
void upPages( PROCESS *p );
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

extern page_directory_t *current_directory;
extern u32int initial_esp;
PRIVATE void move_stack(void *new_stack_start, u32int size);

void scheduler_init(int withPriority) {
    _cli();
    log(L_DEBUG, "PRE. ESP:0x%x, EBP:0x%x", _ESP, _EBP);
    //move_stack((void*)0xE0000000, 0x2000);
    log(L_DEBUG, "POST. ESP:0x%x, EBP:0x%x", _ESP, _EBP);
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
            current->status = BLOCKED;
            current->waitingFlags = W_CHILD;
            current->lastCalled = 0;
            switchProcess();
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
    if (p != current) {
        if (current != NULL) {
            downPages(current);
        }
        current = p;
        upPages(current);
    }
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

PRIVATE void move_stack(void *new_stack_start, u32int size)
{
  u32int i;
  // Allocate some space for the new stack.
  for( i = (u32int)new_stack_start;
       i >= ((u32int)new_stack_start-size);
       i -= 0x1000)
  {
    // General-purpose stack is in user-mode.
    alloc_frame( get_page(i, 1, current_directory), 0 /* User mode */, 1 /* Is writable */ );
  }
  
  // Flush the TLB by reading and writing the page directory address again.
  u32int pd_addr;
  __asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
  __asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

  // Old ESP and EBP, read from registers.
  u32int old_stack_pointer; __asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
  u32int old_base_pointer;  __asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

  // Offset to add to old stack addresses to get a new stack address.
  u32int offset            = (u32int)new_stack_start - initial_esp;

  // New ESP and EBP.
  u32int new_stack_pointer = old_stack_pointer + offset;
  u32int new_base_pointer  = old_base_pointer  + offset;

  // Copy the stack.
  memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp-old_stack_pointer);

  // Backtrace through the original stack, copying new values into
  // the new stack.  
  for(i = (u32int)new_stack_start; i > (u32int)new_stack_start-size; i -= 4)
  {
    u32int tmp = * (u32int*)i;
    // If the value of tmp is inside the range of the old stack, assume it is a base pointer
    // and remap it. This will unfortunately remap ANY value in this range, whether they are
    // base pointers or not.
    if (( old_stack_pointer < tmp) && (tmp < initial_esp))
    {
      tmp = tmp + offset;
      u32int *tmp2 = (u32int*)i;
      *tmp2 = tmp;
    }
  }

  // Change stacks.
  __asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
  __asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}

// a partir de un proceso dado setea como presentes o ausentes todas las paginas de un proceso ademas 
// de las paginas de sus ancestros

void flushPages	( PROCESS *process , int action )
{
	PROCESS *proc_parent;
	int pages, mem_dir, p;
	page_t *page;

    if (process == NULL)
        return;

	pages = process->stacksize / PAGE_SIZE; // cuantas paginas tiene ese proceso
	//direccion de memoria donde comienza el stack ( operacion inversa de create process )
	mem_dir = process->stack;
	for( p=0; p< pages ; ++p ) {
		page = get_page( mem_dir,0,current_directory );
		page->present = action ; // DISABLE or ENABLE
		mem_dir += PAGE_SIZE; 	// 4kb step!
	}
	if(process->parent > 1)
	{
		proc_parent = scheduler_getProcess(process->parent);
		flushPages( proc_parent, action );
	}
}

void downPages( PROCESS *p )
{
  flushPages( p, 0 );
  // Bajo las paginas del proceso actual que pasa a ser antiguo
}

void upPages( PROCESS *p )
{
  flushPages( p, 1 );
  // levanto las paginas del proceso actual
}

