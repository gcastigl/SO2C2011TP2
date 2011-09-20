#include "kasm.h"
#include "defs.h"
#include "kc.h"
#define MAX(a,b) ((a)>(b))?(a):(b)

DESCR_INT idt[0xF];	
GDTR* gdtr = (GDTR*)0x1234;	/* GDTR */
IDTR idtr;					/* IDTR */


extern char scancodes[];
PROCESS procesos[64];
PROCESS idle;
BUFFERTYPE buffer={0};
TTY tty[8];
int CurrentPID=0;
static int nextPID=1;


int CurrentTTY;

char* backupTerm;
int GetPID(void);


static void SetupIDT();
static void SetupGDT();
void loadScanCodes(void);
int LoadStackFrame(int(*process)(int,char**),int argc,char** argv, int bottom, void(*cleaner)());
int NoProcesses(void);

//Retorna True si no hay procesos libres
int NoProcesses()
{
	int i;
	for (i=0;i<64;i++)
	{
		if (procesos[i].free==0)
			return 0;
	}
	return 1;
}




//Retorna un proceso a partir de su nombre
PROCESS* GetProcessByName (char* proceso)
{
	int i, longitud;
	
	if (memcmp2(proceso, "Idle", 4))
	{
		return &idle;
	}
	longitud = strlen2(proceso);
	for(i=0;i<64;i++)
	{
		if (memcmp2(proceso, procesos[i].name, MAX(longitud, strlen2(procesos[i].name))))
		{
			return &procesos[i];
		}
	}
	return 0;
}





//Retorna la TTY del proceso que se esta ejecutando
int GetTTY()
{
	PROCESS* temp;
	temp=GetProcessByPID(CurrentPID);
	return temp->tty;


}

//Retorna un proceso a partir de su PID
PROCESS* GetProcessByPID (int pid)
{
	char* video=(char*)0xb8000;
	int i;
	//video[2080+contador*2]=pid+0x30;
	
	if (pid==0)
	{
		return &idle;
	}
	for(i=0;i<64;i++)
	{
		if (procesos[i].pid== pid)
		{
			return &procesos[i];
		}
	}
	return 0;
}

//Retorna un PID no utilizado
int GetPID(void)
{
	return nextPID++;
}


//Retorna el buffer de la TTY correspondiente al proceso
BUFFERTYPE* GetBuffer (void)
{
	//video[162]=buffer.head+0x30;
	//video[164]=buffer.tail+0x30;
	int mitty=GetTTY();
	return &(tty[mitty].buffer);
}






//funcion creadora
void kmain(void)
{	
		char* video=(char*)0xb8000;
		static int a=0;
		int i;
		_Cli();
			clear_vid();
			SetupIDT();
			SetupGDT();
			buffer.head=0;
			buffer.tail=0;
			_mascaraPIC1(0xFC);
			_mascaraPIC2(0xFF);
			SetupScheduler();
			CurrentTTY=0;
			for(i=0;i<8;i++)
				tty[i].terminal=(char*)CallocVid(160*25);
			backupTerm=tty[0].terminal;
			tty[0].terminal=(char*)0xb8000;
			backupTerm=(char*)Calloc(160*25);
			//CreateProcessAt(Idle,1,0,0x400,2,1);
			CreateProcessAt("Shell 0",Shell,0,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 1",Shell,1,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 2",Shell,2,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 3",Shell,3,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 4",Shell,4,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 5",Shell,5,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 6",Shell,6,0,(char**)0,0x400,2,1);
			CreateProcessAt("Shell 7",Shell,7,0,(char**)0,0x400,2,1);
			
		_Sti();
		while(1)
		{

		}
			
	
}

//Funcion que crea procesos
void CreateProcessAt(char* name, int (*process)(int,char**),int tty, int argc, char** argv, int stacklength, int priority, int isFront)
{
	PROCESS* newprocess;
	char* video =(char*) 0xb8000;
	int i;
	void* stack=Malloc(stacklength);
	for(i=0;i<64;i++)
	{
		if(procesos[i].free==1)
			break;
	}
	procesos[i].pid=GetPID();
	procesos[i].foreground=isFront;
	procesos[i].priority=priority;
	memcpy2(procesos[i].name,name,strlen2(name)+1);
	procesos[i].sleep=0;
	procesos[i].blocked=0;
	procesos[i].tty=tty;
	procesos[i].lastCalled=0;
	procesos[i].stacksize=stacklength;
	procesos[i].stackstart=(int)stack;
	procesos[i].free=0;
	procesos[i].ESP=LoadStackFrame(process,argc,argv,(int)(stack+stacklength-1),Cleaner);
	procesos[i].parent=0;
	if(isFront && CurrentPID>=1)
	{
		PROCESS* proc=GetProcessByPID(CurrentPID);
		char Men[10];
		proc->blocked=2;
		procesos[i].parent=CurrentPID;
	}
	//video[2080]=procesos[i].pid+0x30;
	
}



//Funcion que arma los stack frames de cada proceso
int LoadStackFrame(int(*process)(int,char**),int argc,char** argv, int bottom, void(*cleaner)())
{
	STACK_FRAME* frame= (STACK_FRAME*)(bottom-sizeof(STACK_FRAME));
	frame->EBP=0;
	frame->EIP=(int)process;
	frame->CS=0x08;
	
	frame->EFLAGS=0;
	frame->retaddr=cleaner;
	frame->argc=argc;
	frame->argv=argv;
	return (int)frame;
}

//Funciones que cargan IDT y GDT
static void SetupIDT()
{
	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0);	
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0);	
	setup_IDT_entry (&idt[0x06], 0x08, (dword)&_invop_hand, ACS_INT, 0);	
	setup_IDT_entry (&idt[0x0C], 0x08, (dword)&_ssf_hand, ACS_INT, 0);	
	setup_IDT_entry (&idt[0x0D], 0x08, (dword)&_gpf_hand, ACS_INT, 0);	
	setup_IDT_entry (&idt[0x0B], 0x08, (dword)&_snp_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x05], 0x08, (dword)&_bounds_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x00], 0x08, (dword)&_div0_hand, ACS_INT, 0);
	idtr.base = 0;  
	idtr.base += (dword) &idt;
	idtr.limit = sizeof(idt) - 1;
	_lidt(&idtr);
}

static void SetupGDT()
{
	_getgdt();
	setup_GDT_entry ((DESCR_SEG*)(gdtr->base + gdtr->limit + 1), ((dword)0x10)<<4, 0x00FF, ACS_STACK, 0xC0);
	setup_GDT_entry ((DESCR_SEG*)(gdtr->base + gdtr->limit + 9), 0, 0xFFFFF, ACS_STACK & 0x7F, 0xC0);
	/* Carga de GDTR */
	gdtr->limit += 16;
	_lgdt(gdtr);
}




//Funcion que carga el interpretador de scancodes.
void loadScanCodes(void)
{
	scancodes[30]='a';
	scancodes[48]='b';
	scancodes[46]='c';
	scancodes[32]='d';
	scancodes[18]='e';
	scancodes[33]='f';
	scancodes[34]='g';
	scancodes[35]='h';
	scancodes[23]='i';
	scancodes[36]='j';
	scancodes[37]='k';
	scancodes[38]='l';
	scancodes[50]='m';
	scancodes[49]='n';
	scancodes[24]='o';
	scancodes[25]='p';
	scancodes[16]='q';
	scancodes[19]='r';
	scancodes[31]='s';
	scancodes[20]='t';
	scancodes[22]='u';
	scancodes[47]='v';
	scancodes[17]='w';
	scancodes[45]='x';
	scancodes[21]='y';
	scancodes[44]='z';
	scancodes[57]=' ';
	scancodes[41]='`';
	scancodes[2]='1';
	scancodes[3]='2';
	scancodes[4]='3';
	scancodes[5]='4';
	scancodes[6]='5';
	scancodes[7]='6';
	scancodes[8]='7';
	scancodes[9]='8';
	scancodes[10]='9';
	scancodes[11]='0';
	scancodes[12]='-';
	scancodes[13]='=';
	scancodes[26]='[';
	scancodes[27]=']';
	scancodes[43]='\\';
	scancodes[39]=';';
	scancodes[40]='\'';
	scancodes[51]=',';
	scancodes[52]='.';
	scancodes[53]='/';

	scancodes[130]='A';
	scancodes[148]='B';
	scancodes[146]='C';
	scancodes[132]='D';
	scancodes[118]='E';
	scancodes[133]='F';
	scancodes[134]='G';
	scancodes[135]='H';
	scancodes[123]='I';
	scancodes[136]='J';
	scancodes[137]='K';
	scancodes[138]='L';
	scancodes[150]='M';
	scancodes[149]='N';
	scancodes[124]='O';
	scancodes[125]='P';
	scancodes[116]='Q';
	scancodes[119]='R';
	scancodes[131]='S';
	scancodes[120]='T';
	scancodes[122]='U';
	scancodes[147]='V';
	scancodes[117]='W';
	scancodes[145]='X';
	scancodes[121]='Y';
	scancodes[144]='Z';
	scancodes[157]=' ';
	scancodes[141]='~';
	scancodes[102]='!';
	scancodes[103]='@';
	scancodes[104]='#';
	scancodes[105]='$';
	scancodes[106]='%';
	scancodes[107]='^';
	scancodes[108]='&';
	scancodes[109]='*';
	scancodes[110]='(';
	scancodes[111]=')';
	scancodes[112]='_';
	scancodes[113]='+';
	scancodes[126]='{';
	scancodes[127]='}';
	scancodes[143]='|';
	scancodes[139]=':';
	scancodes[140]='"';
	scancodes[151]='<';
	scancodes[152]='>';
	scancodes[153]='?';
}



// Manejo del timer tick
void int_08() {
}



// Manejo de teclado
void int_09 (int a)
{
	char* video= (char*)0xb8000;
	static int tags=0;
	
	if((a== 0x2A || a== 0x36))
	{
		tags=tags | SHIFT_PRESSED;
		return;
	}
	if(a == 0xAA || a == 0xB6) // Break de los Shifts 
	{
		tags=tags & ~SHIFT_PRESSED;
		return;
	}
	if((a== 0x1d || a== 0xE036))
	{
		tags=tags | CONTROL_PRESSED;
		return;
	}
	if(a == 0x9d || a == 0xB6) // Break de los controls 
	{
		tags=tags & ~CONTROL_PRESSED;
		return;
	}
	if((a== 0x38 || a== 0x54))
	{
		tags=tags | ALT_PRESSED;
		return;
	}
	if(a == 0xB8 || a == 0xD4) // Break de los alts 
	{
		tags=tags & ~ALT_PRESSED;
		return;
	}
	
	
	if(a>=0x3B && a<=0x42 && tags & ALT_PRESSED) // entre F1 y F8
	{
		int nextTTY;
		nextTTY=a-0x3B;
		memcpy2(backupTerm,(char*)0xb8000,160*25);
		memcpy2((char*)0xb8000,tty[nextTTY].terminal,160*25);
		tty[CurrentTTY].terminal=backupTerm;
		backupTerm=tty[nextTTY].terminal;
		tty[nextTTY].terminal=(char*)0xb8000;
		CurrentTTY=nextTTY;
		return;
	}
	if (a & 0x80)
		return;
	
	if(tty[CurrentTTY].buffer.head==(tty[CurrentTTY].buffer.tail+1)%10)
	{
//		video[0]=scancodes[a];
	}
	else
	{
		int i;
		

		tty[CurrentTTY].buffer.buffer[tty[CurrentTTY].buffer.tail]= a+tags*(1<<8);
		tty[CurrentTTY].buffer.tail=(tty[CurrentTTY].buffer.tail+1)%10;
		for (i=0;i<64;i++)
		{
			if(procesos[i].blocked == 1 && procesos[i].tty==CurrentTTY)
			{			
				procesos[i].blocked = 0;

			}

		}
	}
	
	return;
}




//Funcion que hace dormir a una funcion por un periodo de tiempo
void Sleep(int n)
{
	PROCESS* proc;
	_Cli();
		proc=GetProcessByPID(CurrentPID);
		proc->sleep=n*-20;
	_Sti();
	_int8();
}



//Funcion que destruye un proceso a partir de su PID
void Destroy(int PID)
{
	PROCESS* proc;
	PROCESS* padre;
	int test;
	char temp[10];
	char temp2[100];
	itoa2(PID,temp);
	proc=GetProcessByPID(PID);
	if (!proc->free)
	{
		memcpy2(temp2,"Killed->",8);
		test=strlen2(proc->name)+8;
		memcpy2(&temp2[8],proc->name,test-8);
		temp2[test++]='\0';
		puterr(temp2);
		tty[proc->tty].buffer.head=0;
		tty[proc->tty].buffer.tail=0;
		if (proc->parent!=0)
		{
			padre=GetProcessByPID(proc->parent);
			padre->blocked=0;	
		}
		proc->free = 1;
	}
	else
		puterr("Proceso Inexistente");
}
