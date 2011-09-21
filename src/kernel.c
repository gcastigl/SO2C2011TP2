#include "../include/kernel.h"

DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;						/* IDTR */

void doubleFlagsFix(double n);

kmain() {
	int i,num;
	
	/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */
	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0);
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&_int_80_hand, ACS_INT, 0);
	/* Carga de IDTR    */
	idtr.base = 0;
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;

	_lidt (&idtr);

	_Cli();

	/* Habilito interrupcion de timer tick*/
	//_mascaraPIC1(INT_08 & INT_09 & INT_80);
	_mascaraPIC1(0x00);
	_mascaraPIC2(NONE);
	_Sti();
	
	initKeyBoard();
	initVideo();
	initShell();
	doubleFlagsFix(1.1);
	while (1) {
		updateShell();
	}
	
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

