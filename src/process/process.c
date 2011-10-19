#include <process/process.h>

extern PROCESS process[];
extern int nextPID; 
extern int currentPID;
int count100 = 1;
int firstTime = true;
int schedulerActive = false;

void clean(void);
extern int loadStackFrame();
void saveESP(int oldESP);
extern int getNextPID();
int idle_p(int argc, char **argv);
int idle_p2(int argc, char **argv);

void initScheduler(void) {
	int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process[i].slotStatus = FREE;
    }
    //createProcess("Idle", &idle_p, 0, NULL, DEFAULT_STACK_SIZE, &clean, -1, BACKGROUND, READY);
    schedulerActive = true;
}

void createProcess(char* name, int (*processFunc)(int,char**), int argc, char** argv, int stacklength, void (*cleaner)(void), int tty,
    int groundness, int status) {
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
    
    for (i = 0; i<argc; i++){
	    newProcess->argv[i]=(char*)kmalloc(strlen(argv[i])+1);
	    strcpy(newProcess->argv[i],argv[i]);
	}
	
	memcpy(newProcess->name, name, strlen(name) + 1);
	newProcess->pid = getNextPID();
	newProcess->stacksize = stacklength;
	newProcess->stackstart = (int) stack + stacklength - 1;
	newProcess->ESP = loadStackFrame(processFunc, newProcess->stackstart, argc, newProcess->argv, cleaner);
    newProcess->tty = tty;
	newProcess->lastCalled = 0;
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
    
    return;
}

PROCESS* getNextTask(void) {
    int i;
    int nextReady = 0, taskLevel = 0;
    PROCESS *proc;
    
    for (i = 0; i < MAX_PROCESSES; i++) {
        proc=&process[i];
        if ((proc->slotStatus != FREE) && proc->status == READY) {
            if (proc->lastCalled >= taskLevel) {
                nextReady=i;
                taskLevel = proc->lastCalled;
                proc->lastCalled = -1;
            } else {
                proc->lastCalled++;
            }
        }
    }
    return &process[nextReady];

    //notar que si no hay procesos disponibles, retornara &processes[0], o sea idle

}

void logAllESP() {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].slotStatus == OCCUPIED) {
            log(L_INFO, "Stack for %s: %d", process[i].name, process[i].ESP);
        }
    }
}
/*
int getNextProcess(int oldESP) {
    PROCESS *proc, *proc2;
    proc2 = getProcessByPID(currentPID);
    log(L_INFO, "Switching task with old ESP %d. Current %s", oldESP, (proc2 == NULL ? "NONE" : proc2->name));
    proc = getNextTask();
    saveESP(oldESP);
    logAllESP();
    currentPID = proc->pid;
    log(L_INFO, "Returning stack %d", proc->ESP);
    return proc->ESP;
}
*/

int getNextProcess(int oldESP) {
    PROCESS *proc;
    int newESP;
    proc = getProcessByPID(currentPID);
    proc->status = READY;
    proc = getNextTask();
    log(L_INFO, "Next task: %s", proc->name);
    if (currentPID != proc->pid) {
        if (firstTime == false) {
            saveESP(oldESP); // el oldESP esta el stack pointer del proceso
        } else {
            firstTime = false;
        }
        currentPID = proc->pid;
        proc->status = RUNNING;
        newESP = proc->ESP;
    } else {
        newESP = oldESP;
    }
    
    return newESP;
}

void saveESP (int oldESP) {
    PROCESS *proc;
    proc = getProcessByPID(currentPID);
    if (proc != NULL) {
        proc->ESP = oldESP;
    }
    return;
}

void clean(void) {   
    PROCESS *temp;
	//int i;
    temp = getProcessByPID(currentPID);
    log(L_INFO, "Cleaning process %s", temp->name);
	temp->slotStatus = FREE;
    temp = getProcessByPID(temp->parent);
	if (temp != NULL) {
	    if (temp->status == CHILD_WAIT) {
            temp->status = READY;
	    }
	}
	
    switchProcess();
	//kfree((void*)temp->stackstart-temp->stacksize+1);
	
	//for (i=0;i<temp->argc;i++)
        //kfree((void*)temp->argv[i]);
}

//Processes

int idle_p(int argc, char **argv) {
    while(1) {}
    return 0;
}

int tty_p(int argc, char **argv) {
    int index = initTTY();
    while(1) {
        shell_update(index);
    }
}