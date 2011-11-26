#include "../include/scheduler.h"


//extern PROCESST init;
extern PROCESST processes[];
extern int currentPID;
int count100=1;


//Funcion que almacena el ESP actual

void saveESP (int oldESP);
PROCESST* getNextTask(void);
int stackAlmostFull( );

int
getNextProcess( int oldESP )
{
    PROCESST *proc;

    saveESP( oldESP ); // el oldESP esta el stack pointer del proceso
        
    proc = getNextTask();
    
    downPages( currentPID );     // baja las paginas del proceso viejo y todos sus ancestros

    currentPID = proc->pid;
    
    upPages( currentPID );     // levanta las paginas de este proceso y la de sus ancestros
 
    return proc->ESP;
}

void
saveESP (int oldESP)
{
    PROCESST *proc;
    proc = getProcessByPID (currentPID);
    
    proc->ESP = oldESP;
    return;
}

PROCESST* 
getNextTask(void)
{
    int i;
    int nextReady=0, taskLevel=0;
    PROCESST *proc;

    for (i = 1; i < MAXPROCESS; i++)
    {
        proc=&processes[i];
	
	if ( (proc->freeSlot != FREE) && (proc->blocked==NOTBLOCKED) )//si es un proceso no bloqueado
	{
            if (proc->lastCalled>=taskLevel)
	    {
               nextReady=i; //me quedo con el que hace mas tiempo que no se ejecuto
               taskLevel=proc->lastCalled;
            }
            proc->lastCalled++; //incremento el tiempo en que no se ejecuto en todos los procesos
	}
	else if ( (proc->freeSlot != FREE) && (proc->blocked==SLEEPING))
	{
	    if ( --(proc->sleep)==0)
		proc->blocked=NOTBLOCKED;
	}

	if (count100 == 100)
	{
	    proc->cpu=proc->countExec;
	    proc->countExec=0;

	}
    }

    if ( (++count100)>100)
    {
	processes[0].cpu=processes[0].countExec;
	processes[0].countExec=0;

	count100=1;
    }
    processes[nextReady].countExec++; //Sera el proceso que se ejecutara a continuacion
    processes[nextReady].lastCalled=0; //Sera el proceso que se ejecutara a continuacion

    return &processes[nextReady];

    //notar que si no hay procesos disponibles, retornara &processes[0], o sea init :)

}

// verifica si al stack del proceso actual le queda menos de 4kb disponible

// int stackAlmostFull( )
// {
// 	PROCESST * proc;
// 	proc=getProcessByPID(currentPID);
// 
//  	return ( ( proc->ESP - proc->stackstart + proc->stacksize - 1 ) < 0x1000 );
// }
// 


//Inicializa la tarea idle
void
initScheduler (void)
{
	int i;	

    // todos los slots estan libres 
    for (i = 0; i < MAXPROCESS; i++)
        processes[i].freeSlot = FREE;

}
