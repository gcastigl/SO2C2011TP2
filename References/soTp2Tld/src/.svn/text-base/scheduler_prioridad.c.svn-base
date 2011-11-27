#include "../include/scheduler.h"

#define FUN_PRIORIDAD ( (proc->lastCalled)*( 100*(3-proc->priority) +1) )


//extern PROCESST init;
extern PROCESST processes[];
extern int currentPID;
int count100=1;


//Funcion que almacena el ESP actual

void saveESP (int oldESP);
PROCESST* getNextTask(void);



int
getNextProcess(int oldESP)
{
    PROCESST *proc;

    saveESP (oldESP);

    proc = getNextTask();

    // baja el proceso viejo
    downPages( currentPID ); 

    currentPID = proc->pid;

    // levanta las paginas de este proceso
    upPages( currentPID );

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
    int nextReady=0, taskLevel=0,priorityLevel=4;
    PROCESST *proc;

    for (i = 1; i < MAXPROCESS; i++)
    {
        proc=&processes[i];
	
	if ( (proc->freeSlot != FREE) && (proc->blocked==NOTBLOCKED) )//si es un proceso no bloqueado
	{
            if (proc->lastCalled==taskLevel)
	    {
		if (proc->priority<priorityLevel)
		{
		    nextReady=i;
		    priorityLevel=proc->priority;
		}
            }else if (proc->lastCalled>taskLevel)
	    {
		    nextReady=i;
		    taskLevel=proc->lastCalled;
		    priorityLevel=proc->priority;  
	    }
            proc->lastCalled+=(4-proc->priority); //incremento el tiempo en que no se ejecuto en todos los procesos
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


//Inicializa la tarea idle
void
initScheduler (void)
{
	int i;	

    // todos los slots estan libres 
    for (i = 0; i < MAXPROCESS; i++)
        processes[i].freeSlot = FREE;

}
/*
void
initScheduler (void) {
    int i,j;

 //   void *stack = Malloc (512);
   //Hay que busfcar una direccion perteneciente a la zona de kernel
    void *stack = (char *) 0x300000;

    // todos los slots estan libres 
    for (i = 0; i < MAXPROCESOS; i++)
        procesos[i].free_slot = 1;

    //no hay procesos en la lista de sleep 
    for (i = 0; i < MAXPROCESOS; i++)
        peticionesDormir[i].time = 0;

    //Levantamos los ultimos 4MB de memoria para shared-mem 
     for( i = maxmem/4096-1; i <= maxmem/4096-1; i++)
    {
       page_table = page_directory + 4096 * (i+1);

       for( j = 0 ; j < 1024 ; j++)
         page_table[j] = page_table[j] | 7;

       page_directory[i] = (unsigned long)page_table;
       page_directory[i] = page_directory[i] | 7;

     } 
}



void SetupScheduler(void)
{
	void* idleprocess;
	int i;

	for (i=0;i<64;i++)
		procesos[i].free=1;
	
	idleprocess= Malloc(0x200);
	idle.pid=0;
	idle.foreground=0;
	idle.priority=4;
	memcpy2(idle.name,"Idle",5);
	idle.sleep=0;
	idle.blocked=0;
	idle.tty=0;
	idle.stackstart=(int)idleprocess;
	idle.stacksize=0x200;
	procesos[i].parent=0;
	idle.lastCalled=0;
	idle.ESP=LoadStackFrame(Idle,0,0,(int)(idleprocess+0x1FF),Cleaner);
	
	return;
}

//Funcion a donde van a parar las funciones que terminan.
void Cleaner(void)
{
	char Men[10];
	Cli();
	Destroy(CurrentPID);
	k_clear_screen();
	mess("==>");
	Sti();
	while(1);	
}

*/




/*
//Funcion que obtiene el ESP de idle para switchear entre tareas.
void* GetTemporaryESP (void)
{
	return (void*)idle.ESP;
}

//Funcion que devuelve el PROCESS* siguiente a ejecutar
PROCESS* GetNextProcess (void)
{
	
	char* video=(char*)0xb8000;
	PROCESS* temp;
	//selecciona la tarea
	temp=GetNextTask();
	temp->lastCalled=0;
	CurrentPID=temp->pid;
	last100[counter100]=CurrentPID;
	counter100=(counter100+1)%100;
	return temp;
	
}

//Funcion Scheduler
PROCESS* GetNextTask()
{
	int i;
	int winner;
	int winnervalue=-1;
	if (NoProcesses())
		return &idle;
	for(i=0;i<64;i++)
	{
		if(!procesos[i].free)
		{
			if(procesos[i].sleep>=0)
			{
				if(procesos[i].blocked == 0)
				{
					procesos[i].lastCalled++;
					int temp;
					temp=(MAX_PRIORITY-procesos[i].priority)*PRIORITY_RATIO+procesos[i].lastCalled;
					if (temp>winnervalue)
					{
						winnervalue=temp;
						winner=i;
					}
				}
			}
			else
				procesos[i].sleep++;
		}
	}
	if (winnervalue>-1)
		return &procesos[winner];
	else
		return &idle;
}

//Funcion que devuelve el ESP del proceso actual.
int LoadESP(PROCESS* proc)
{
	return proc->ESP;
}

//Funcion a donde van a parar las funciones que terminan.
void Cleaner(void)
{
	char Men[10];
	Cli();
	Destroy(CurrentPID);
	k_clear_screen();
	mess("==>");
	Sti();
	while(1);
	
}

//Funcion que inicializa la tarea IDLE
void SetupScheduler(void)
{
	void* idleprocess;
	int i;

	for (i=0;i<64;i++)
		procesos[i].free=1;
	
	idleprocess= Malloc(0x200);
	idle.pid=0;
	idle.foreground=0;
	idle.priority=4;
	memcpy2(idle.name,"Idle",5);
	idle.sleep=0;
	idle.blocked=0;
	idle.tty=0;
	idle.stackstart=(int)idleprocess;
	idle.stacksize=0x200;
	procesos[i].parent=0;
	idle.lastCalled=0;
	idle.ESP=LoadStackFrame(Idle,0,0,(int)(idleprocess+0x1FF),Cleaner);
	
	return;
}
*/

