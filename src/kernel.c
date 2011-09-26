#include "../include/kernel.h"

static DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
static IDTR idtr;						/* IDTR */
int currentPid = 0;
int nextPid = 0;
extern PROCESS process[];
void doubleFlagsFix(double n);
void setupIDT();

kmain() {
	_Cli();
		setupIDT();
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		doubleFlagsFix(1.1);
		initKeyBoard();
		initVideo();
        setupScheduler();
		setFD(3);
		createProcessAt("Shell 1", shell_process, 0, 0, NULL, 0x400, 2, 1);
	_Sti();
    while (1);
	
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

int getCurrPid() {
	return currentPid;
}

int setCurrPid(int pid) {
    currentPid = pid;
}

int getNextPid() {
	return nextPid++;
}

/*
 *	fd = File descriptor referring to the open file.
 *	buffer = Storage location for data.
 *	count = Maximum number of bytes.
 */
size_t __read(int fd, void * buffer, size_t count) {
	_SysCall(SYSTEM_READ,fd, buffer, count);
	return count;
}

/*
 *	fd = File descriptor of file into which data is written.
 *	buffer = Data to be written..
 *	count = Maximum number of bytes.
 */
size_t __write(int fd, const void * buffer, size_t count) {
	_SysCall(SYSTEM_WRITE, fd, buffer, count);
	return count;
}

void doubleFlagsFix(double n) {
    //Does nothing
}

boolean noProcesses() {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].free != 1) {
            return FALSE;
        }
    }
    
    return TRUE;
}