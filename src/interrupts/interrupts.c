#include <interrupts/interrupts.h>

//Timer Tick
void int_08() {
    switch_task();
}

//Keyboard
void int_09() {
	handleScanCode(inb(0x60));
}

void int_80(int sysCallNumber, void ** args) {
	switch(sysCallNumber) {
		case SYSTEM_WRITE:
			sysWrite((int) args[0], args[1], (int)args[2]);
			break;
		case SYSTEM_READ:
			sysRead((int) args[0], args[1],(int)args[2]);
			break;
	}
}

void bounds() {
	panic("Bounds Exceeded!", TRUE);
}

void invop() {
	panic("Invalid OPCODE", TRUE);
}

void div0() {
	panic("Division by 0", TRUE);
}


void gpf() {
	panic("General Protection Fault", TRUE);
}

void ssf() {
	panic("Stack Segment Fault", TRUE);
}


void snp() {
	panic("Segment Not Present", TRUE);
}

void pageFault() {
    panic("Page fault!", TRUE);
}