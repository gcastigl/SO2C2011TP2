#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESS_NAME 	32
#define MAX_PROCESSES		64

typedef struct {
	int pid;
	char name[MAX_PROCESS_NAME];
	int priority;
	int tty;
	int foreground;
	int lastCalled;
	int sleep;
	int blocked;
	int parent;
	int ESP;
	int free;
	int stackstart;
	int stacksize;
} PROCESS;

typedef struct {
	int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,  EIP, CS, EFLAGS;
	void*retaddr;
	int argc;
	char** argv;
} STACK_FRAME;

void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront);

int getPID(void);

PROCESS* getProcessByPID(int pid);

#endif
