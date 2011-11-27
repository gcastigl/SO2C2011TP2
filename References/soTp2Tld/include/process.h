#ifndef _PROCESS_H
#define _PROCESS_H

#include "kernel.h"
#include "time.h"
#include "shell.h"
//#include "stdio.h"

#define MAXNAME 30
#define MAXARG 10
#define PID_INIT 1
#define SHELL_INIT 2
#define MAXPROCESS 64
#define MAXTERMS 6
#define WAIT_CHILD  2
#define MAXLINE 80

#define STACK_USER 0x4000
#define PRIORITY 3

enum {NOTBLOCKED,BLOCKED,CHILD_WAIT,SLEEPING};
enum {NOTFREE,FREE};
enum {FRONTGROUND, BACKGROUND};

typedef struct
{
	int pid;
	char name [MAXNAME];
	int priority;
	int tty;
	int background; 
	int lastCalled;
	int sleep;
	int blocked;
	int parent;
	int ESP;
	int freeSlot;
	int stackstart;
	int stacksize;
	int cpu;
	int countExec; //Acumulador de ejecuciones utilizado para calcular el porcentaje de cpu
	int argc;
	char * argv[MAXARG];
	void *info;
        
} PROCESST;

typedef struct
{
    int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
} STACK_FRAME;

//Funcion que crea un proceso
void createProcess(char* name, int (*process)(int,char**),int tty, int argc, char** argv, int stacklength, int priority, int inBackground,int blocked);
//Devuelve en pid
int getPID(void);
//Retorna un proceso dado un pid
PROCESST * getProcessByPID (int pid);

//Dado un nombre de un proceso lo busca en el arreglo y lo devuelve
PROCESST *getProcessByName (char * process);

//Dado un tty, retorna el proceso "mas viejo" de esa consola
PROCESST * getParentProcessByTty (int tty);

//Proceso init
int initProcess(int argc, char **argv);
//Bloquea el proceso que tiene el pid dado
int blockProcess (int pid, int blockCode, int blockAux);
//Desbloquea un proceso si esta en estado blockCode,dado su pid
int unblockProcessFrom (int pid,int blockCode);
//Funcion que es llamada cuando termina un proceso
void clean (void);


// proceso hijo que lee del stack del padre
int parentTouch(int argc, char **argv);

int stackIsFull ( void );

// proceso que baja las paginas del proceso ejm1 y luego trata de leer de la direccion de su stack
int down_fault(int argc, char **argv);

// proceso que muestra las direcciones de memoria de variables alocadas y desalocadas en memoria
int mem_use(int argc, char **argv);

// proceso que muestra las direcciones de memoria de variables alocadas y desalocadas en memoria
int prueba(int argc, char **argv);


// proceso que se llama recursivamente a si mismo con el doble del stack
int growStack(int argc, char **argv);

/*Proceso que imprime la hora*/
int clock(int argc, char **argv);

/*Proceso que limpia la pantalla*/
int clear(int argc,char ** argv);

/*Proceso que imprime la lista de comandos*/
int help(int argc,char ** argv);

/*Proceso que cambia el idioma del teclado*/
int setKwd(int argc,char **argv);

/*Proceso que muestra los dispositivos pci*/
int showPci(int argc,char **argv);

/*Proceso que realiza la funcion top*/
int top(int argc,char **argv);

/*Proceso para probar las prioridades*/
int priorityTest(int argc,char **argv);


void Kill (PROCESST *proc);

void killChilds (int pidParent);

void execute (char *name, int (*process) (int argc, char **argv), int inBack);

void sleep(int ciclos);

int printStackPosition(int argc,char **argv);

int showStack(int argc,char **argv);

int priorityTest(int argc,char **argv);


#endif
