#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <lib/kheap.h>
#include <util/logger.h>

#define DEFAULT_STACK_SIZE 0x400
// I think there is one like this already defined...
#define MAX_PROCESS_NAME 	16
#define MAX_PROCESSES		32
#define MAX_ARG             16
#define P_RATIO             2

typedef enum {BLOCKED = 0, READY, RUNNING} status;
typedef enum {BACKGROUND = 0, FOREGROUND} groundness;
typedef enum {PNONE = 0, VERY_LOW, LOW, NORMAL, HIGH, VERY_HIGH, SKY_HIGH} priority;
typedef enum {W_FIFO = 0} block_type;

#define MAX_PRIORITY HIGH
#define MAX_FILES_PER_PROCESS	10

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
	int stackstart;
	// Process state
	int pid;
	int parent;
	int groundness;
	int priority;
	int sleep;
	int status;
	int lastCalled;
	int waitingFlags;
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
