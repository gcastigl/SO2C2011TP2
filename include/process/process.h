#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <lib/kheap.h>
#include <util/roundRobin.h>
#include <util/logger.h>

#define DEFAULT_STACK_SIZE 0x400
// I think there is one like this already defined...
#define MAX_PROCESS_NAME 	32
#define MAX_PROCESSES		64
#define MAX_ARG             32
#define P_RATIO             2

enum {READY, BLOCKED, RUNNING};
enum {BACKGROUND, FOREGROUND};
enum {PNONE, VERY_LOW, LOW, NORMAL, HIGH, VERY_HIGH, SHELL_HIGH = 15};

#define MAX_PRIORITY HIGH
#define MAX_FILES_PER_PROCESS	10

typedef struct {
	char name[MAX_NAME_LENGTH];
	u32int mask;	// inode type
	u32int count;	// number of links
	u32int inode;	// inode number
	u32int mode;
	u32int offset;
	//u32int op;
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
	// I/O
	int tty;
	file_descriptor_entry fd_table[MAX_FILES_PER_PROCESS];
} PROCESS;

/*
* - Arreglo que contiene los últimos 100 pids ejecutados
**/
int last100[100];

void process_initialize();

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
*
**/
void process_create(char* name, int (*processFunc)(int,char**), int argc, char** argv,
		int stacklength, void (*cleaner)(void), int tty, int groundness, int status, int priority);

/* getProcessByPID
*
* Recibe como parametros:
* - Process ID
*
* Devuelve el proceso con el pid indicado o NULL si el pid es inválido
**/
PROCESS* process_getPID(int pid);


/* getCurrentPID
*
* Devuelve el PID del proceso actual
**/
int process_currentPID();

void process_setCurrent(int pid);

/* clean
*
* Función cementerio al cual van a parar todos los procesos una vez que terminan
**/
void clean();

/* kill
*
* Recibe como parametros:
* - pid del proceso a terminar
*
* Mata el proceso indicado por pid
**/
void process_kill(int pid);

/* getCurrentProcess
*
* Devuelve el proceso actual
**/
PROCESS *process_getCurrent();

/* setPriority
*
* Recibe como parametros:
* - pid
* - nueva prioridad
*
* Setea una nueva prioridad para el pid dado
**/
void setPriority(int pid, int newPriority);

// FIXME: PORQUE NO COMPILA CON RoundRobin* ?????!!!!
void* process_getActive();

void* process_getBlocked();

boolean process_setStatus(u32int pid, u32int status);

// =============================================================
// 							SCHEDULER
// =============================================================


/* initScheduler
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Inicializa el scheduler (multitasking)
**/
void scheduler_init(int withPriority);

void scheduler_setActive(boolean active);

boolean scheduler_isActive();

#endif
