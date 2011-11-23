#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <memory/kheap.h>

#define DEFAULT_STACK_SIZE 0x2000

#define _EIP 	read_eip()
#define _CS 	read_cs()
#define _SS 	read_ss()
#define _DS 	read_ds()

#define INIFL 	0x200
#define FP_SEG(fptr) 	((unsigned)((unsigned long)(fptr) >> 16))
#define FP_OFF(fptr)	((unsigned)(fptr))

// I think there is one like this already defined...
#define MAX_PROCESS_NAME 		16
#define MAX_PROCESSES			32
#define MAX_ARG             	16
#define P_RATIO             	2
#define MAX_FILES_PER_PROCESS	10

typedef enum {PNONE = 0, VERY_LOW, LOW, NORMAL, HIGH, VERY_HIGH, SKY_HIGH} priority_t;
typedef enum {BACKGROUND = 0, FOREGROUND} 	groundness_t;
typedef enum {BLOCKED = 0, READY, RUNNING, FINILIZED} 	status_t;
typedef enum {W_FIFO = 0, W_INPUT} 			block_t;


typedef struct {
	char name[MAX_NAME_LENGTH];
	u32int mask;	// inode type
	u32int count;	// number of links
	u32int inode;	// inode number
	u32int mode;
	u32int offset;
	u32int length;
} file_descriptor_entry;

typedef struct {
	char name[MAX_PROCESS_NAME];
	// Permission
	int ownerUid;
	// Memory status
	int argc;
	char *argv[MAX_ARG];
	int ESP;
	int stacksize;
	int stack;
	// Process state
	int pid;
	int parent;
	groundness_t groundness;
	priority_t priority;
	status_t status;
	int lastCalled;
	block_t waitingFlags;
	int waitingInfo;
	// I/O
	int tty;
	file_descriptor_entry fd_table[MAX_FILES_PER_PROCESS];
} PROCESS;

/* createProcess
*
* Recibe como parametros:
* - Nombre del proceso
* - Función a ejecutar (proceso)
* - Cantidad de argumentos
* - Argumentos
* - Largo del stack
* - TTY que lo está creando
* - Groundness (Foreground/Background)
* - Status en el cual inicializarlo
* - Prioridad del proceso para el scheduler
* - pid del proceso padre
**/
void process_initialize(PROCESS* newProcess, char* name, int(*processFunc)(int, char**),
		int argc, char** argv, int stacklength, void(*cleaner)(void),
		int tty, int groundness, int status, int priority, int parentPID);

void process_finalize(PROCESS* newProcess);

u32int yield();

u32int fork();

#endif
