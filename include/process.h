#ifndef PROCESS_H_
#define PROCESS_H_

#include "defs.h"

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
	int esp;
	int free;
	int stackStart;
	int stackSize;
} PROCESS;

typedef struct {
	int edi, esi, ebp, esp, ebx, edx, ecx, eax, eip, cs, eFlags;
	void* retAddr;
	int argc;
	char** argv;
} STACK_FRAME;

void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront);

void destroyProcess(int pid);
PROCESS* getProcessByPid(int pid);
PROCESS* getTempProcess();
int loadStackFrame(int(*process)(int, char**), int argc, char** argv, int bottom, void(*destroyProcess)(int));
#endif
