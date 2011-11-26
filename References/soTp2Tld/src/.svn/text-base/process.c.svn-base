#include "../include/process.h"

extern PROCESST processes[];
extern nextPID; 
extern currentPID;

extern int flag;

//extern char param;
void stackStatus();

void 
createProcess(char* name, int (*process)(int,char**),int tty, int argc, char** argv, int stacklength, int priority, int inBackground,int blocked)
{
	PROCESST * this;
	int i;
	int stack;

	for(i=0;i< MAXPROCESS ;i++) //Busco un slot libre
	{
		if(processes[i].freeSlot==FREE)
			break;
	}

	if (i==MAXPROCESS) //No hay lugar para otro proceso
	    return;

	this=&processes[i];

	//aloco memoria para el stack, devuelve una direccion de memoria
	stack=kmalloc_a( stacklength ); // tamanio en bytes del stack


	for (i=0;i<argc;i++){
	    this->argv[i]=(char*)kmalloc(strlen(argv[i])+1);
	    strcpy(this->argv[i],argv[i]);
	}

	this->pid=getPID();
	this->background=inBackground;
	this->priority=priority;
	strcpy(this->name,name);
	this->sleep=0;
	this->blocked=blocked;
	this->tty=tty;
	this->lastCalled=0;
	this->stacksize=stacklength;
	this->stackstart=(int)stack + stacklength - 1;
	this->freeSlot=NOTFREE;
	this->argc=argc;
	this->ESP =_ArmaStackFrame (process,this->stackstart,argc,this->argv, clean);
	this->parent=currentPID;
	this->cpu=0;
	this->countExec=0;


	if(inBackground==FRONTGROUND)
	{
		PROCESST* proc;
		if (proc=getProcessByPID(currentPID))
		{
		    //Bloqueado esperando que termine su hijo
		    proc->blocked=CHILD_WAIT;
		    proc->lastCalled=0;
		    proc->cpu=0;
		    _switchProcess();
		}
	}	
}

//Retorna un nuevo PID
int getPID(void)
{
	return nextPID++;
}


int
blockProcess (int pid, int blockCode, int blockAux) 
{
    int i;

    	for (i = 0; i < MAXPROCESS; i++) 
	{
        	if (processes[i].pid == pid && processes[i].freeSlot==NOTFREE) 
		{
			if (blockAux==-1 || processes[i].blocked==blockAux)
			    processes[i].blocked = blockCode;
            		break;
        	}
    	}

    	return 0;   
}

int
unblockProcessFrom (int pid,int blockCode) 
{
    int i;
    for (i = 0; i < MAXPROCESS; i++) 
    {
       	if (processes[i].pid == pid && processes[i].freeSlot==NOTFREE) 
        {
	  if (blockCode==-1 || processes[i].blocked == blockCode)
	      processes[i].blocked = NOTBLOCKED;
          break;
        }
    }

    return 0;
}

//Retorna un proceso a partir de su PID
PROCESST *
getProcessByPID (int pid)
{
	int i;
	
	for(i=0;i< MAXPROCESS ;i++)
	{
		if (processes[i].freeSlot==NOTFREE && processes[i].pid== pid )
		{
			return &processes[i]; //Encontro el proceso
		}
	}

	return 0; //Pid Invalido
}

PROCESST *
getProcessByName (char * process) 
{
    	int i;

   	 for (i = 0; i < MAXPROCESS; i++) 
	{
        	if (processes[i].freeSlot==NOTFREE && strcmp (process, processes[i].name) == 0) 
            		return &processes[i];
        	
    	}

	//No se encontro el proceso	
    	return 0;
}

PROCESST *
getParentProcessByTty (int tty) 
{
    	int i;
	PROCESST *proc;

   	for (i = 0; i < MAXPROCESS; i++) 
	{
        	if (processes[i].freeSlot==NOTFREE && processes[i].tty==tty) 
		{
		  proc=&processes[i];
		  while (proc->parent!=0)
		  {
		      if ((proc=getProcessByPID(proc->parent))==0) //no existe el padre
			  return 0;
  
		  }
		  return proc;
        	}
  
    	}

	//No se encontro el proceso	
    	return 0;
}

void
clean (void) 
{   
    	PROCESST *temp;
    	PROCESST *parent;
	int i;
  
	//Obtengo el proceso actual
    	temp = getProcessByPID (currentPID);
	

	if(temp->background==FRONTGROUND && (parent=getProcessByPID(temp->parent)) )
	    if (parent->blocked==CHILD_WAIT)
		parent->blocked = NOTBLOCKED;
    
	temp->freeSlot = FREE;

	kfree(temp->stackstart-temp->stacksize+1); //operacion inversa a processes[i].stackstart=(int)stack + stacklength - 1;
	
	for (i=0;i<temp->argc;i++)
	  kfree((int)temp->argv[i]);

	_switchProcess();
}




/****CONTINUACION DE CODIGO PARA LAS LLAMADAS DE INTERRUPCION 85********/

void
Kill (PROCESST *proc) 
{
    	PROCESST * parent;

    	if (proc->pid == PID_INIT || proc->pid == SHELL_INIT)
	{
	    puts("Proceso ");
	    puts(proc->name);
	    puts(". No puede matarse\n");
	    return;
	}

	puts("Kill del proceso ");
	puts(proc->name);
	puts("\n");				

	
	proc->freeSlot = FREE;
	kfree(proc->stackstart-proc->stacksize+1); //operacion inversa a processes[i].stackstart=(int)stack + stacklength - 1;
	
  int i;
	for (i=0;i<proc->argc;i++)
	  kfree((int)proc->argv[i]);

	killChilds(proc->pid);		

	if(proc->background==FRONTGROUND && (parent=getProcessByPID(proc->parent)) )
	    if (parent->blocked==CHILD_WAIT)
		parent->blocked = NOTBLOCKED;
	
	return;
}

void
killChilds (int pidParent) 
{
    	int i;

    	for (i = 0; i < MAXPROCESS; i++) 
	{
       		//Mata hijos en front
        	if (processes[i].freeSlot==NOTFREE && processes[i].parent == pidParent && processes[i].background == FRONTGROUND) 
			Kill (&processes[i]);
    	}

}

void
execute (char *name, int (*process) (int argc, char **argv), int inBack) 
{
    createProcess (name, process, getProcessTty(), 0, (char**)0,STACK_USER,PRIORITY,inBack,NOTBLOCKED);
}



void
sleep(int count)
{
  PROCESST *proc;

  proc=getProcessByPID(currentPID);

  proc->blocked=SLEEPING;

  proc->sleep=count;

  _switchProcess();

}


/****PROCESOS*****/

int 
initProcess(int argc, char **argv)
{
    while(1);
}

#define PLUS_STACK (0x2000) /** 8 kb **/

void expandStack( void )
{
	PROCESST * proc;
	int newStack,stack,deltaStack;
	int i,ebp,esp,posESP,posEBP,relEBP,esp_temp,dirRegEBP;

	proc=getProcessByPID( currentPID );

	ebp=_readEBP();
	esp=_readESP();

	stack=proc->stackstart;

	// pido un nuevo buffer con dos paginas mas 8 kb
	newStack=kmalloc_a( proc->stacksize + PLUS_STACK );
	newStack+=(proc->stacksize + PLUS_STACK-1);

	deltaStack=newStack-stack;
	
	// copio el stack usado al nuevo buffer mas grande
	for( i=0; stack-i>=esp ;i+=4 ){
	      *(int*)(newStack-i)=*(int*)(stack-i);
	}
		
	// calculo las posiciones relativas de esp y ebp para actualizar las referencias del nuevo stack
	posESP=stack-esp;
	posEBP=stack-ebp;
	relEBP=ebp-esp;
	
	dirRegEBP=newStack-posEBP;
		
	// actualizo los EBPs
	while ( *(int*)ebp != 0 )
	{
	      *(int*)(newStack-posEBP)+=deltaStack;
	      posEBP-=(*(int*)ebp-ebp);
	      ebp=*(int*)ebp;
	}

	// actualizo la informacion relacionada al stack del proceso en curso
	proc->stacksize+=PLUS_STACK;
	proc->stackstart=newStack;
	proc->ESP+=deltaStack;

	// cambio los registros del procesador ESP, EBP
	_writeStackReg( newStack-posESP, dirRegEBP );
	
	// libero el stack anterior
	kfree(stack-proc->stacksize+PLUS_STACK+1);
}

void stackStatus()
{
	int esp=_readESP();
	PROCESST * proc;
	proc=getProcessByPID(currentPID);
		
	puts(" Start: ");
	printHex_int( proc->stackstart );

	puts(" ESP: ");  	setColor(HELP_TXT);
	printHex_int( esp );	setColor(WHITE_TXT);

	puts(" Stop: ");
	printHex_int( proc->stackstart - proc->stacksize + 1 );

	puts(" Usado: ");
	printHex_int( proc->stackstart - esp );
	
	putc('\n');

}

int stackIsFull ()
{
	PROCESST * proc;
	int esp=_readESP();
	proc=getProcessByPID(currentPID);

	stackStatus();
	
 	if( ( esp - proc->stackstart + proc->stacksize - 1 ) < 0x1000 ){
	    puts("\n Stack Overflow! ==> Quedan menos de 4kb en stack!\n\n");
	    return 1;
	}
	return 0;
  
}

int growRec( int reg )
{
	if( reg )
	{
	    puts(" growREC");
	    if( stackIsFull() )
	    {
		_Cli(); 
		expandStack();
		_Sti();
	    }
	    {
		int arreglo[0x200]; // 1 kb * sizeof(int) =  2 kb
		growRec( --reg );
	    }
	}
}

int growStack(int argc, char **argv)
{
	growRec( 12 ); 
}



int mem_use(int argc, char **argv)
{
	
	int *a = (int*)kmalloc(sizeof(int));
	int *b = (int*)kmalloc(sizeof(int));
	int *c = (int*)kmalloc(2*sizeof(int));
	puts("\nSe pide memoria para 2 ints (a,b) y una arreglo c de 2 ints \n");
	puts(" a: ");
	printHex_int((int)a);
	puts("\n b: ");
	printHex_int((int)b);
	puts("\n c: ");
	printHex_int((int)c);
	puts("\nSe libera c y b\n");
	kfree((int)c);
	kfree((int)b);
	puts("\nSe aloca un int d\n");
	int *d = (int*)kmalloc(sizeof(int));
	puts("\n d: ");
	printHex_int((int)d);
	puts("\n");
/*	*a=37;
	*b=32;
	c[0]=24;
	c[1]=56;
	*d=77;*/
	kfree((int)a);
/*	puts("\nSe le asignan los valores:\n");*/
/*	printDec(*a); puts("\n");
	printDec(*b); puts("\n");
	printDec(c[0]); puts("\n");
	printDec(c[1]); puts("\n");*/
// 	printDec(*d); 
	puts("\nsizeof(int):      ");
	printDec_int(sizeof(int)); puts("\nsizeof(header_t): ");
	printDec_int(sizeof(header_t)); puts("\nsizeof(footer_t): ");
	printDec_int(sizeof(footer_t)); puts("\n");
	
}

int down_fault(int argc, char **argv)
{
 	_Cli();
	
	int i;
	int *mem_proc;
	putc('\n');
	for(i=0;i< MAXPROCESS ;i++)
	{
		if(processes[i].freeSlot!=FREE)
		{
			printDec(processes[i].pid);
			putc(' ');
			puts(processes[i].name);
			puts(" stack: ");
			printHex_int(processes[i].stackstart-processes[i].stacksize+1);
			puts(" size: ");
			printHex_int(processes[i].stacksize);
			putc('\n');						
		}
		if(!strcmp(processes[i].name,"shell2"))
		{
			mem_proc=(int*)(processes[i].stackstart-processes[i].stacksize+1);
			downPages( processes[i].pid );
		}
	}
	putc('\n');
	puts(" se bajan las paginas del proceso shell2 y la de sus ancestros\n");
	puts(" luego se quiere escribir leer de la dir (" );
	printHex_int( (int)mem_proc );
	puts(") \n\n");
	i=*mem_proc;
	
	_Sti();
}


int parentTouch(int argc, char **argv)
{

	int *fault;
	int mem_parent=0;
	int i,value;
	int *ptr;
	
	putc('\n');
	
	for(i=0;i< MAXPROCESS ;i++)
		if(processes[i].freeSlot!=FREE)	
	{
		if(!strcmp(processes[i].name,"shell1"))
		{
			processes[i].info=(void*)1;
			mem_parent=processes[i].stackstart-processes[i].stacksize+1;
		}
		printDec(processes[i].pid);		putc(' ');
		puts(processes[i].name);		puts(" stack: ");
		printHex_int(processes[i].stackstart-processes[i].stacksize+1);	puts(" size: ");
		printHex_int(processes[i].stacksize); 	puts(" son of: ");
		printDec(processes[i].parent);		puts(" info: ");
		printHex_int((int)processes[i].info);	putc('\n');	
	}
	
	puts( "\n\n Desde el proceso hijo parentTouch se lee de la direccion (" );
	printHex_int( mem_parent );
	puts( ") del proceso padre shell1\n\n" );
	
	ptr=(int*)mem_parent;
}


int prueba(int argc, char** argv )
{
	_exec("growStack",growStack,FRONTGROUND);
}

int 
top(int argc,char **argv)
{
    PROCESST *proc;
    int i;

    char line[MAX_LINE];

    clearScreen();

    do
    {

      updateScreen();
    
      puts("                                    TOP\n");
      puts("          Procesos y porcentaje de CPU utilizada. ('q' para salir)\n\n");
      puts("         PID             CPU%             ESTADO             NOMBRE\n");
            
      for (i = 0; i < MAXPROCESS; i++) 
      {
        proc=&processes[i];
        if (proc->freeSlot == NOTFREE) 
        {
          puts("         ");
          printDec(proc->pid);
          puts("               ");
          printDec(proc->cpu);
          puts("       ");
          switch(proc->blocked)
          {
              case NOTBLOCKED:
            puts("Listo para ejecutar");
            break;
              case BLOCKED:
            puts("     Bloqueado     ");
            break;
              case CHILD_WAIT:
            puts("   Esperando hijo  ");
            break;
              case SLEEPING:
            puts("     Durmiendo     ");
            break;
          }
    
          puts("        "); 
          puts(proc->name); 
          puts("       \n");
          
        }
      }

      puts("                                                                     \n");	   
      puts("\n");
	
      _sleep(80);	
    }while (!isCharIn('q'));
}

int
clock(int argc, char **argv)
{
      int count=0;

      while(1)
      {
	    getTime();
	    _sleep(5);
      }

      return 0;
}

int
clear(int argc,char ** argv)
{
 
	clearScreen();

}

int
help(int argc,char ** argv)
{
      puts_title("Comandos:");
      
      setColor(HELP_TXT);    puts(" clear   ");
      setColor(WHITE_TXT);   puts("  - limpia pantalla\n");
      setColor(HELP_TXT);    puts(" kwd     ");
      setColor(WHITE_TXT);   puts("  - cambia idioma del teclado\n");
      setColor(HELP_TXT);    puts(" pci     ");
      setColor(WHITE_TXT);   puts("  - muestra dispositivos pci\n");
      setColor(HELP_TXT);    puts(" killall <name>");
      setColor(WHITE_TXT);   puts("  - mata procesos con nombre <name>\n");
      setColor(HELP_TXT);    puts(" kill <pid>");
      setColor(WHITE_TXT);   puts("- mata un proceso\n");
      setColor(HELP_TXT);    puts(" top     ");
      setColor(WHITE_TXT);   puts("  - procesos y porcentaje de CPU utilizada.\n");	
      setColor(HELP_TXT);    puts(" printStack");
      setColor(WHITE_TXT);   puts("  - proceso que imprime la direccion de memoria de su stack. \n");	
      setColor(HELP_TXT);    puts(" priority");
      setColor(WHITE_TXT);   puts("  - muestra implementacion del scheduler con prioridad\n\n");
	
      setColor(HELP_TXT);    puts(" me      ");		
      setColor(WHITE_TXT);   puts("  * TEST Uso de memoria\n");
      setColor(HELP_TXT);    puts(" chp     ");		
      setColor(WHITE_TXT);   puts("  * TEST baja paginas de memoria e intenta leer de ellas\n");
      setColor(HELP_TXT);    puts(" parent  ");
      setColor(WHITE_TXT);   puts("  * TEST crea un proceso hijo de Shell1, y este hijo lee de su stack\n");
      setColor(HELP_TXT);    puts(" growStack");
      setColor(WHITE_TXT);   puts(" * TEST proceso que agranda su stack\n");

      putc('\n');
}

int
setKwd(int argc,char **argv)
{
    language=!language;
   
    if(language)
    {
	puts("Teclado en Ingl");
	putc(e_ACENT);
	puts("s\n\n");
    }	 
    else
    {   
	puts("Teclado en Espa");
	putc(ENIEMINUS);
	puts("ol\n\n");
	}
}

int
showPci(int argc,char **argv)
{
  lspci();
}

int
printStackPosition(int argc,char **argv)
{
	PROCESST * p;
	p=getProcessByPID(currentPID);
	puts("Comienzo del stack: ");
	printHex_int(p->stackstart);
	puts("\n");
	//while(1);
}

int
showStack(int argc,char **argv)
{
	int esp,ebp,cols=1,i=0;
	PROCESST * p;
	
	p=getProcessByPID(currentPID);
	esp=_readESP();
	ebp=_readEBP();
	
	puts("PROCESST: ");printHex_int( (int) p);puts("\n");
	puts("argv: ");printHex_int( (int) argv);puts("\n");

	puts("\nReg EBP: ");	printHex_int(ebp);  		puts(" "); 
	puts("*ebp: ");  	printHex_int(*(int*)ebp);	puts(" ");
	puts("EBP POS:"); 	printDec(ebp-esp);		puts("\n");
	
	puts("\nSTACK FRAME:\n" );
	do
	{
		printHex_int( esp+i ); puts("["); printDec(i); puts("]=");setColor(HELP_TXT);
		printHex_int( *(int*)(esp+i) );	puts(" ");		setColor(WHITE_TXT);

		if( cols==3 ) {
		  puts("\n");
		  cols=0;
		}
		cols++; i+=4;;
		
	} while( esp+i < p->stackstart+4 );
	
	if( cols == 2 || cols ==3 )
	  puts("\n");

}


int
priorityTest(int argc,char **argv)
{
  puts("\nProceso con prioridad: ");
  puts(argv[0]);
  while(1);
}

