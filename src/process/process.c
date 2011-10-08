#include <process.h>

extern PROCESS processes[];
extern nextPID; 
extern currentPID;

void clean(void);

void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront) {
	PROCESS* newprocess;
	void* stack = kmalloc_a(stacklength);
	int i;
	for(i = 0; i < MAX_PROCESSES;i++) {
		if(process[i].free == 1)
			break;
	}
    newProcess = &process[i];
	
	newProcess->pid = getPID();
	newProcess->foreground = isFront;
	newProcess->priority = priority;
	memcpy(newProcess->name, name, strlen(name) + 1);
	newProcess->sleep = 0;
	newProcess->blocked = 0;
	newProcess->tty = tty;
	newProcess->lastCalled = 0;
	newProcess->stacksize = stacklength;
	newProcess->stackstart = (int) stack;
	newProcess->free = 0;
	newProcess->ESP = loadStackFrame(process, argc, argv,(int)(stack+stacklength-1), clean);
	newProcess->parent = 0;
	int currPID = getCurrPID();
	if(isFront && currPID >= 1 && 0) {
		PROCESS* proc = getProcessByPID(currPID);
		proc->blocked = 2;
		newProcess->parent = currPID;
	}
}

void clean(void) {   
    PROCESS *temp;
	int i;
	//Obtengo el proceso actual
    temp = getProcessByPID (currentPID);

	temp->freeSlot = FREE;

	kfree(temp->stackstart-temp->stacksize+1); //operacion inversa a processes[i].stackstart=(int)stack + stacklength - 1;
	
	for (i=0;i<temp->argc;i++)
	  kfree((int)temp->argv[i]);
}
