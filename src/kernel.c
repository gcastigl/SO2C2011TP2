#include "../include/kernel.h"

static DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
static IDTR idtr;					/* IDTR */
static int currentPID = 0;
static int nextPID = 1;

void doubleFlagsFix(double n);
void setupIDT();

int kmain() {
	_Cli();
		setupIDT();
		nextPID = 0;
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		doubleFlagsFix(1.1);
		keyboard_init();
		video_init();
		tty_init();
		setFD(3);
		shell_init();
		_initTTCounter();
	_Sti();
	while (1) {
		updateShell();
	}
	return 0;
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

int getCurrPID() {
	return currentPID;
}

int getNextPID() {
	return nextPID++;
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
}

