#include "../include/process.h"

static PROCESS procesos[MAX_PROCESS];
static PROCESS idle;

void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront) {
	PROCESS* newprocess;
	void* stack = malloc(stacklength);
	int i;
	for(i = 0; i < MAX_PROCESS;i++) {
		if(procesos[i].free == 1)
			break;
	}
	procesos[i].pid = getPID();
	procesos[i].foreground = isFront;
	procesos[i].priority = priority;
	memcpy(procesos[i].name, name, strlen(name) + 1);
	procesos[i].sleep = 0;
	procesos[i].blocked = 0;
	procesos[i].tty = tty;
	procesos[i].lastCalled = 0;
	procesos[i].stacksize = stacklength;
	procesos[i].stackstart = (int) stack;
	procesos[i].free = 0;
	// procesos[i].ESP = loadStackFrame(process, argc, argv,(int)(stack+stacklength-1), Cleaner);
	procesos[i].parent = 0;
	int currPID = getCurrPID();
	if(isFront && currPID >= 1 && 0) {
		PROCESS* proc = getProcessByPID(currPID);
		proc->blocked = 2;
		procesos[i].parent = currPID;
	}
}

// Retorna un proceso a partir de su PID
PROCESS* getProcessByPID(int pid) {
	int i;
	if (pid==0) {
		return &idle;
	}
	for(i = 0;i < MAX_PROCESS; i++) {
		if (procesos[i].pid == pid) {
			return &procesos[i];
		}
	}
	return 0;
}
