#include <process/process.h>
#include <session.h>

extern PROCESS process[];
extern int nextPID;
extern int currentPID;
int count100;
int firstTime = true;
int schedulerActive = false;
static int usePriority;
static int activeProcesses;
extern int loadStackFrame();

/* saveESP
*
* Recibe como parametros:
* - valor del viejo ESP
*
* Guarda el ESP del proceso actual
**/
void saveESP(int oldESP);
extern int getNextPID();

void initScheduler(int withPriority) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process[i].slotStatus = FREE;
    }
    count100 = 0;
    activeProcesses = 0;
    usePriority = withPriority;
    createProcess("Idle", &idle_cmd, 0, NULL, DEFAULT_STACK_SIZE, &clean, -1, BACKGROUND, READY, VERY_LOW);
    schedulerActive = true;
}

void createProcess(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, void(*cleaner)(void), int tty,
        int groundness, int status, int priority) {
    int i;
    PROCESS *newProcess;
    newProcess = kmalloc(sizeof(PROCESS));
    void *stack = kmalloc(stacklength);

    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].slotStatus == FREE) {
            break;
        }
    }

    if (i == MAX_PROCESSES) {
        return;
    }

    newProcess = &process[i];

    for (i = 0; i < argc; i++) {
        newProcess->argv[i] = (char*) kmalloc(strlen(argv[i])+1);
        strcpy(newProcess->argv[i], argv[i]);
    }

    memcpy(newProcess->name, name, strlen(name) + 1);
    newProcess->ownerUid = session_getEuid();
    newProcess->pid = getNextPID();
    newProcess->stacksize = stacklength;
    newProcess->stackstart = (int) stack + stacklength - 1;
    newProcess->ESP = loadStackFrame(processFunc, newProcess->stackstart, argc,
            newProcess->argv, cleaner);
    newProcess->tty = tty;
    newProcess->lastCalled = 0;
    newProcess->priority = priority;
    newProcess->groundness = groundness;
    newProcess->slotStatus = OCCUPIED;
    newProcess->parent = currentPID;
    newProcess->status = status;

    if (groundness == FOREGROUND) {
        PROCESS *p;
        p = getProcessByPID(currentPID);
        if (p != NULL) {
            p->status = CHILD_WAIT;
            p->lastCalled = 0;
            switchProcess();
        }
    }
    activeProcesses++;
    return;
}

PROCESS* getNextTask(int withPriority) {
    int i;
    int nextReady = 0;
    int temp, bestScore = 0;
    PROCESS *proc;

    for (i = 1; i < MAX_PROCESSES; i++) {
        proc=&process[i];
        if ((proc->slotStatus != FREE) && (proc->status != CHILD_WAIT)) {
            proc->lastCalled++;
            if (withPriority == true) {
                temp = proc->priority * P_RATIO + proc->lastCalled;
            } else {
                temp = proc->lastCalled;
            }
            if (proc->priority == PNONE) {
                temp /= 5;
            }
            if (temp > bestScore) {
                bestScore = temp;
                nextReady = i;
            }
        }
    }
    last100[(count100 = (count100 + 1) % 100)] = nextReady;
    return &process[nextReady];

    //notar que si no hay procesos disponibles, retornara &processes[0], o sea idle

}

int getNextProcess(int oldESP) {
    PROCESS *proc, *proc2;
    proc2 = getProcessByPID(currentPID);
    if (proc2->status == RUNNING) {
        proc2->status = READY;
    }
    proc = getNextTask(usePriority);
    proc->status = RUNNING;
    proc->lastCalled = 0;
    if (firstTime == false) {
        saveESP(oldESP); // el oldESP esta el stack pointer del proceso
    } else {
        firstTime = false;
    }
    currentPID = proc->pid;

    return proc->ESP;
}

void saveESP(int oldESP) {
    PROCESS *proc;
    proc = getProcessByPID(currentPID);
    if (proc != NULL) {
        proc->ESP = oldESP;
    }
    return;
}

int getCurrentPID(void) {
    return currentPID;
}

PROCESS *getCurrentProcess(void) {
    return getProcessByPID(currentPID);
}

void killChildren(int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].slotStatus == OCCUPIED) {
            if (process[i].parent == pid) {
                kill(process[i].pid);
            }
        }
    }
}

void kill(int pid) {
    int i;
    PROCESS *p = NULL;
    PROCESS *parent;
    if (pid < MAX_TTYs) {
        return;
    }
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].slotStatus == OCCUPIED) {
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
        }
    }

    if (p == NULL) {
        printf("PID is not valid\n");
        return;
    }

    killChildren(pid);

    p->slotStatus = FREE;
    parent = getProcessByPID(p->parent);
    if (parent) {
        if (parent->status == CHILD_WAIT) {
            parent->status = READY;
        }
    }
    activeProcesses--;
    //Free process memory
}

void setPriority(int pid, int newPriority) {
    PROCESS *p = getProcessByPID(pid);
    p->priority = newPriority;
}

void clean(void) {

    PROCESS *temp;
    int i;
    temp = getProcessByPID(currentPID);
    temp->slotStatus = FREE;
    temp = getProcessByPID(temp->parent);
    if (temp != NULL) {
        if (temp->status == CHILD_WAIT) {
            temp->status = READY;
        }
    }
    activeProcesses--;
    kfree((void*) temp->stackstart - temp->stacksize + 1);

    for (i = 0; i < temp->argc; i++)
        kfree((void*) temp->argv[i]);
    switchProcess();
}
