#include "../include/process.h"

PROCESS process[MAX_PROCESSES];
PROCESS idle;

void createProcessAt(char* name, int (*f_process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront) {
	void* stack = (void*)malloc(stacklength);
	int i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		if(process[i].free == 1)
			break;
	}
	process[i].pid = getNextPid();
	process[i].foreground = isFront;
	process[i].priority = priority;
	memcpy(process[i].name, name, strlen(name) + 1);
	process[i].sleep = 0;
	process[i].blocked = 0;
	process[i].tty = tty;
	process[i].lastCalled = 0;
	process[i].stackSize = stacklength;
	process[i].stackStart = (int) stack;
	process[i].free = 0;
	process[i].esp = loadStackFrame(f_process, argc, argv,(int)(stack + stacklength - 1), destroyProcess);
	process[i].parent = 0;
	int currPid = getCurrPid();
	if(isFront && currPid >= 1) {
		PROCESS* proc = getProcessByPid(getCurrPid());
		proc->blocked = TRUE;
		process[i].parent = getCurrPid();
	}
}

int loadStackFrame(int(*process)(int, char**), int argc, char** argv, int bottom, void(*destroyProcess)(int)) {
    STACK_FRAME* frame = (STACK_FRAME*) (bottom - sizeof(STACK_FRAME));
    frame->ebp = 0;
	frame->eip = (int) process;
	frame->cs = 0x08;
	
	frame->eFlags = 0;
	frame->retAddr = destroyProcess;
	frame->argc = argc;
	frame->argv = argv;
	return (int) frame;
}

void destroyProcess(int pid) {
    PROCESS* process = getProcessByPid(pid);
    PROCESS* parent;
    if (process != NULL && !process->free) {
        if (process->parent != 0) {
            parent = getProcessByPid(process->parent);
            parent->blocked = 0;
        }
    } else {
        //Error: Not existant process!
    }
}

PROCESS* getTempProcess() {
    return &idle;
}

// Retorna un proceso a partir de su PID
PROCESS* getProcessByPid(int pid) {
	int i;
	if (pid == 0) {
		return &idle;
	}
	for(i = 0;i < MAX_PROCESSES; i++) {
		if (process[i].pid == pid) {
			return &process[i];
		}
	}
	return NULL;
}

PROCESS* getProcessByName(char* name) {
    int i;
    if (strcmp(name, "idle")) {
        return &idle;
    }
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (strcmp(process[i].name, name)) {
            return &process[i];
        }
    }
}