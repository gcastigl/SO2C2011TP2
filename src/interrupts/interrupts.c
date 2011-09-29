#include <interrupts/interrupts.h>

//Timer Tick
void int_08() {
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
	panic("Bounds Exceeded!");
}

void invop() {
	panic("Invalid OPCODE");
}

void div0() {
	panic("Division by 0");
}


void gpf() {
	panic("General Protection Fault");
}

void ssf() {
	panic("Stack Segment Fault");
}


void snp() {
	panic("Segment Not Present");
}

void pageFault() {
    panic("Page fault!");
}