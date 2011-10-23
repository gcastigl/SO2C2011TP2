#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <lib/kheap.h>
#include <util/logger.h>
#include <tty.h>
#define MAX_PROCESS_NAME 	32
#define MAX_PROCESSES		64
#define MAX_ARG             32
#define P_RATIO             2
enum {READY, BLOCKED, CHILD_WAIT, SLEEPING, RUNNING};
enum {BACKGROUND, FOREGROUND};
enum {OCCUPIED = 0, FREE};
enum {VERY_LOW, LOW, NORMAL, HIGH};

#define MAX_PRIORITY HIGH
typedef struct {
	int pid;
	char name[MAX_PROCESS_NAME];
	int priority;
	int tty;
	int groundness; 
	int lastCalled;
	int sleep;
	int status;
	int parent;
	int ESP;
	int slotStatus;
	int stackstart;
	int stacksize;
	int argc;
	char *argv[MAX_ARG];
} PROCESS;

int last100[100];

#define DEFAULT_STACK_SIZE 0x300

void createProcess(char* name, int (*processFunc)(int,char**), int argc, char** argv, int stacklength, void (*cleaner)(void), int tty,
    int groundness, int status, int priority);
int getPID(void);
PROCESS* getProcessByPID(int pid);
PROCESS* getNextTask(int withPriority);
void initScheduler(int withPriority);
int getActiveProcesses(void);
void clean(void);
void kill(int pid);
#endif
