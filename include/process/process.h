#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <lib/kheap.h>

#define MAX_PROCESS_NAME 	32
#define MAX_PROCESSES		64
#define MAX_ARG             32
#define FREE 1

enum {READY, BLOCKED, CHILD_WAIT, SLEEPING, RUNNING};
enum {BACKGROUND, FOREGROUND};

typedef struct {
	int pid;
	char name [MAX_PROCESS_NAME];
	int priority;
	int tty;
	int foreground; 
	int lastCalled;
	int sleep;
	int status;
	int parent;
	int ESP;
	int freeSlot;
	int stackstart;
	int stacksize;
	int cpu;
	int countExec; //Acumulador de ejecuciones utilizado para calcular el porcentaje de cpu
	int argc;
	char * argv[MAX_ARG];
	void *info;
} PROCESS;

typedef struct {
	int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,  EIP, CS, EFLAGS;
	void*retaddr;
	int argc;
	char** argv;
} STACK_FRAME;

void createProcessAt(char* name, int (*processFunc)(int,char**),int tty, int argc,
    char** argv, int stacklength, int priority, int isFront, int status);

int getPID(void);
PROCESS* getProcessByPID(int pid);
PROCESS* getNextTask(void);
void initScheduler(void);

// Processes
int idle(int argc, char **argv);

#endif
