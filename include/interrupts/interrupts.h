#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <defs.h>
#include <io.h>
#include <command.h>
#include <process/task.h>
#include <asm/interrupts.h>
#include <interrupts/defs.h>

void initBasicHandlers();
extern void _gdt_flush(u32int);
extern void _idt_flush(u32int);
void init_descriptor_tables();
void timerTickHandler(registers_t regs);
void keyboardHandler(registers_t regs);
void systemCallHandler(int sysCallNumber, void ** args);

isr_t interruptHandlers[256];

#endif

