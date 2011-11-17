#include <interrupts/interrupts.h>

extern void switchProcess(void);
int taskSwitch = true;
static int ticksSinceLasfFlush = 0;

char *exceptionString[32] = {
    "Division by zero exception",
    "Debug exception",
    "Non maskable interrupt",
    "Breakpoint exception",
    "Into detected overflow",
    "Out of bounds exception",
    "Invalid opcode exception",
    "No coprocessor exception",
    "Double fault exception",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt exception",
    "Coprocessor fault",
    "Alignment check exception",
    "Machine check exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void disableTaskSwitch() {
    taskSwitch = false;
}

void enableTaskSwitch() {
    taskSwitch = true;
}

//Timer Tick
void timerTickHandler(registers_t regs) {
    _increaseTTCounter();
    if (ticksSinceLasfFlush++ >= TICKS_PER_FLUSH) {
    	cache_flush();
    	ticksSinceLasfFlush = 0;
    }
    if (scheduler_isActive() && taskSwitch) {
		switchProcess();
    }
}

//Keyboard
void keyboardHandler(registers_t regs) {
	handleScanCode(_port_in(0x60));
}


void *systemCallHandler(int sysCallNumber, void ** args) {
    void *ret = NULL;
	switch(sysCallNumber) {
		case SYSTEM_USERLIST:
			do_userlist((calluser_t *)args[0]);
			break;
		case SYSTEM_USERADD:
			do_useradd(args[0], args[1]);
			break;
		case SYSTEM_USERDEL:
			do_userdel(args[0]);
			break;
		case SYSTEM_GROUPLIST:
			do_grouplist((callgroup_t *)args[0]);
			break;
		case SYSTEM_GROUPADD:
			do_groupadd(args[0], args[1]);
			break;
		case SYSTEM_GROUPDEL:
			do_groupdel(args[0]);
			break;
		case SYSTEM_USERSETGID:
			do_usersetgid(args[0], (int) args[1]);
			break;
		case SYSTEM_WRITE:
			sysWrite((int) args[0], args[1], (int)args[2]);
			break;
		case SYSTEM_READ:
			sysRead((int) args[0], args[1],(int)args[2]);
			break;
		case SYSTEM_YIELD:
            switchProcess();
            break;
        case SYSTEM_OPEN:
            ret = (void*)sysOpen((char*)args[0], (int)args[1], (int)args[2]);
            break;
        case SYSTEM_CLOSE:
            ret = (void*)sysClose((char*)args[0], (int)args[1], (int)args[2]);
            break;
	}
	
    return ret;
}
// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t regs) {
    
    if (regs.int_no < 32) {
        log(L_ERROR, "%s (%d): \n\
 ds: %d\n\
 edi: %d\n\
 esi: %d\n\
 ebp: %d\n\
 esp: %d\n\
 ebx: %d\n\
 edx: %d\n\
 ecx: %d\n\
 eax: %d\n\
 eip: %d\n\
 cs: %d\n\
 ss: %d", exceptionString[regs.int_no], regs.err_code, regs.ds, \
regs.edi, regs.esi, regs.ebp, regs.esp, regs.ebx, regs.edx, regs.ecx,\
regs.eax, regs.eip, regs.cs, regs.ss);
        panic(exceptionString[regs.int_no], 1, true);
    }
}

// This gets called from our ASM interrupt handler stub.
void irq_handler(registers_t regs) {
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= 40) {
        // Send reset signal to slave.
        _port_out(0xA0, 0x20);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    _port_out(0x20, 0x20);

    if (interruptHandlers[regs.int_no] != 0)
    {
        isr_t handler = interruptHandlers[regs.int_no];
        handler(regs);
    }
}
