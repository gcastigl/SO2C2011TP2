#include "common.h"
#include "isr.h"
#include "idt.h"

isr_t interrupt_handlers[IDT_SIZE];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_t regs) {
    if(regs.int_no==-128){//cableo orrendo, pero por alguna razon me lo pone negativo
    	regs.int_no*=-1;
    	}
    if (interrupt_handlers[regs.int_no] != NULL) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}

void irq_handler(registers_t regs) {
    if (regs.int_no >= IRQ8) {
        outb(PORT_PIC2, SIGNAL_EOI);
    }
    outb(PORT_PIC1, SIGNAL_EOI);
    isr_handler(regs);
}
