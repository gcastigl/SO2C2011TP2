#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <defs.h>
#include <process/process.h>

/*
* - Arreglo que contiene los últimos 100 pids ejecutados
**/
int last100[100];

/* initScheduler
*
* Recibe como parametros:
* - valor booleano indicando que scheduler usar
*
* Inicializa el scheduler (multitasking)
**/
void scheduler_init(int withPriority);

/* getProcessByPID
*
* Recibe como parametros:
* - Process ID
*
* Devuelve el proceso con el pid indicado o NULL si el pid es inválido
**/
PROCESS* scheduler_getProcess(int pid);


/* getCurrentPID
*
* Devuelve el PID del proceso actual
**/
int scheduler_currentPID();

void scheduler_setCurrent(PROCESS* p);

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
PROCESS *scheduler_getCurrentProcess();

/* setPriority
*
* Recibe como parametros:
* - pid
* - nueva prioridad
*
* Setea una nueva prioridad para el pid dado
**/
void setPriority(int pid, int newPriority);

void scheduler_setStatus(u32int pid, u32int status);

PROCESS **scheduler_getAllProcesses();

void scheduler_setActive(boolean active);

boolean scheduler_isActive();

void scheduler_schedule(char* name, int(*processFunc)(int, char**), int argc,
        char** argv, int stacklength, int tty, int groundness, int status, int priority);

u32int scheduler_activeProcesses();

void scheduler_blockCurrent(block_t waitFlag);

#endif
