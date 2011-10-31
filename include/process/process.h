#ifndef PROCESS_H
#define PROCESS_H

#include <defs.h>
#include <lib/kheap.h>
#include <util/logger.h>

#define MAX_PROCESS_NAME 	32
#define MAX_PROCESSES		64
#define MAX_ARG             32
#define P_RATIO             2

enum {READY, CHILD_WAIT, RUNNING};
enum {BACKGROUND, FOREGROUND};
enum {OCCUPIED = 0, FREE};
enum {PNONE, VERY_LOW, LOW, NORMAL, HIGH, VERY_HIGH, SHELL_HIGH = 15};

#define MAX_PRIORITY HIGH
typedef struct {
	int ownerUid;
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

/*
* - Arreglo que contiene los últimos 100 pids ejecutados
**/
int last100[100];

#define DEFAULT_STACK_SIZE 0x400

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
void createProcess(char* name, int (*processFunc)(int,char**), int argc, char** argv, int stacklength, void (*cleaner)(void), int tty,
    int groundness, int status, int priority);

/* getProcessByPID
*
* Recibe como parametros:
* - Process ID
*
* Devuelve el proceso con el pid indicado o NULL si el pid es inválido
**/
PROCESS* getProcessByPID(int pid);

/* getNextTask
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Devuelve el próximo proceso a ejecutar
**/
PROCESS* getNextTask(int withPriority);

/* initScheduler
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Inicializa el scheduler (multitasking)
**/
void initScheduler(int withPriority);

/* getCurrentPID
*
* Devuelve el PID del proceso actual
**/
int getCurrentPID(void);

/* clean
*
* Función cementerio al cual van a parar todos los procesos una vez que terminan
**/
void clean(void);

/* kill
*
* Recibe como parametros:
* - pid del proceso a terminar
*
* Mata el proceso indicado por pid
**/
void kill(int pid);

/* getCurrentProcess
*
* Devuelve el proceso actual
**/
PROCESS *getCurrentProcess(void);

/* setPriority
*
* Recibe como parametros:
* - pid
* - nueva prioridad
*
* Setea una nueva prioridad para el pid dado
**/
void setPriority(int pid, int newPriority);
#endif
