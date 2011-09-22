#include "../include/kernel.h"

#define MAX_PROCESS	64
#define MAX_TTY		8

DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;						/* IDTR */

static TTY tty[MAX_TTY];
static int currentTTY;
static PROCESS procesos[MAX_PROCESS];
static PROCESS idle;
static int currentPID = 0;
static int nextPID = 1;

void doubleFlagsFix(double n);
void setupIDT();
void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront);

int getPID(void);
PROCESS* getProcessByPID(int pid);

kmain() {
	int i,num;
	_Cli();
	setupIDT();
	currentTTY = 0;
	nextPID = 0;
	/* Habilito interrupcion de timer tick*/
	_mascaraPIC1(0xFC);
	_mascaraPIC2(0xFF);
	doubleFlagsFix(1.1);
	initKeyBoard();

	//initVideo();
	createProcessAt("Shell 0", NULL, 0, 0, NULL, 1 << 10, 2, 1);
	initShell();
	_Sti();
	while (1) {
		updateShell();
	}
	
}

void setupIDT() {
	/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */
	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&_int_80_hand, ACS_INT, 0);
	/*Carga de IDTR*/
	idtr.base = 0;
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;
	_lidt (&idtr);
}

void createProcessAt(char* name, int (*process)(int,char**),int tty, int argc,
	char** argv, int stacklength, int priority, int isFront) {
	PROCESS* newprocess;
	void* stack = malloc(stacklength);
	int i;
	for(i = 0; i < MAX_PROCESS;i++) {
		if(procesos[i].free == 1)
			break;
	}
	procesos[i].pid = getPID();
	procesos[i].foreground = isFront;
	procesos[i].priority = priority;
	memcpy(procesos[i].name, name, strlen(name) + 1);
	procesos[i].sleep = 0;
	procesos[i].blocked = 0;
	procesos[i].tty = tty;
	procesos[i].lastCalled = 0;
	procesos[i].stacksize = stacklength;
	procesos[i].stackstart = (int) stack;
	procesos[i].free = 0;
	// procesos[i].ESP = loadStackFrame(process, argc, argv,(int)(stack+stacklength-1), Cleaner);
	procesos[i].parent = 0;
	if(isFront && currentPID >= 1 && 0) {
		PROCESS* proc = getProcessByPID(currentPID);
		proc->blocked = 2;
		procesos[i].parent = currentPID;
	}
}

// Retorna un PID no utilizado
int getPID() {
	return nextPID++;
}

// Retorna un proceso a partir de su PID
PROCESS* getProcessByPID(int pid) {
	int i;
	if (pid==0) {
		return &idle;
	}
	for(i = 0;i < MAX_PROCESS; i++) {
		if (procesos[i].pid == pid) {
			return &procesos[i];
		}
	}
	return 0;
}


/*
fd = File descriptor referring to the open file.

buffer = Storage location for data.

count = Maximum number of bytes.
*/
size_t __read(int fd, void * buffer, size_t count) {
	_SysCall(SYSTEM_READ,fd, buffer, count);
	return count;
}


/*
fd = File descriptor of file into which data is written.

buffer = Data to be written..

count = Maximum number of bytes.
*/
size_t __write(int fd, const void * buffer, size_t count) {
	_SysCall(SYSTEM_WRITE, fd, buffer, count);
	return count;
}

void doubleFlagsFix(double n) {
}

