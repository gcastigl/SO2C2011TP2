#include <process/process.h>

extern PROCESS process[];
extern int nextPID; 
extern int currentPID;
int count100 = 1;

int schedulerActive = false;

void clean(void);
extern int loadStackFrame();
void saveESP(int oldESP);
extern int getNextPID();

void createProcessAt(char* name, int (*processFunc)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront, int status) {
	PROCESS* newProcess;
	void* stack = kmalloc_a(stacklength);
	int i;
	for(i = 0; i < MAX_PROCESSES;i++) {
		if(process[i].freeSlot == 1)
			break;
	}
	
	if (i == MAX_PROCESSES) {
        return;
	}
	
    newProcess = &process[i];
	
	for (i=0;i<argc;i++){
	    newProcess->argv[i]=(char*)kmalloc(strlen(argv[i])+1);
	    strcpy(newProcess->argv[i],argv[i]);
	}
	
	newProcess->pid = getNextPID();
	newProcess->foreground = isFront;
	newProcess->priority = priority;
	memcpy(newProcess->name, name, strlen(name) + 1);
	newProcess->sleep = 0;
	newProcess->status = status;
	newProcess->tty = tty;
	newProcess->lastCalled = 0;
	newProcess->stacksize = stacklength;
	newProcess->stackstart = (int) stack;
	newProcess->freeSlot = 0;
	newProcess->ESP = loadStackFrame(processFunc, newProcess->stackstart, argc, newProcess->argv, clean);
	newProcess->parent = currentPID;
	if(isFront && currentPID >= 1 && 0) {
		PROCESS* proc = getProcessByPID(currentPID);
		proc->status = BLOCKED;
		newProcess->parent = currentPID;
	}
}

PROCESS* getNextTask(void) {
    int i;
    int nextReady = 0, taskLevel = 0;
    PROCESS *proc;

    for (i = 1; i < MAX_PROCESSES; i++) {
        proc=&process[i];
	
        if ((proc->freeSlot != FREE)) {
            if (proc->lastCalled>=taskLevel) {
                nextReady=i;
                taskLevel=proc->lastCalled;
            }
            proc->lastCalled++;
        } else if ((proc->freeSlot != FREE)) {
            if (--(proc->sleep) == 0)
                proc->status = READY;
    	}

    	if (count100 == 100) {
            proc->cpu = proc->countExec;
            proc->countExec = 0;
	    }
    }

    if ((++count100) > 100) {
        process[0].cpu = process[0].countExec;
        process[0].countExec = 0;
        count100 = 1;
    }
    process[nextReady].countExec++;
    process[nextReady].lastCalled = 0;

    return &process[nextReady];

    //notar que si no hay procesos disponibles, retornara &processes[0], o sea init :)

}

int getNextProcess(int oldESP) {
    PROCESS *proc;
    saveESP(oldESP); // el oldESP esta el stack pointer del proceso
    proc = getNextTask();
    
    downPages(currentPID);     // baja las paginas del proceso viejo y todos sus ancestros
    currentPID = proc->pid;
    upPages(currentPID);     // levanta las paginas de este proceso y la de sus ancestros
    
    return proc->ESP;
}

void saveESP (int oldESP) {
    PROCESS *proc;
    proc = getProcessByPID(currentPID);
    
    proc->ESP = oldESP;
    return;
}

void initScheduler(void) {
	int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process[i].freeSlot = FREE;
    }
    schedulerActive = true;
}

void clean(void) {   
    PROCESS *temp;
	int i;
	//Obtengo el proceso actual
    temp = getProcessByPID (currentPID);

	temp->freeSlot = FREE;

	kfree((void*)temp->stackstart-temp->stacksize+1);
	
	for (i=0;i<temp->argc;i++)
	  kfree((void*)temp->argv[i]);
}

//Processes

int idle(int argc, char **argv) {
    while(1);
}