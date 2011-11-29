#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <defs.h>
#include <interrupts/defs.h>
#include <util/logger.h>
#include <asm/interrupts.h>
#include <lib/file.h>
#include <io.h>


void initBasicHandlers();

extern void _gdt_flush(u32int);

extern void _idt_flush(u32int);

void init_descriptor_tables();

void registerInterruptHandler(u8int number, isr_t handler);

void timerTickHandler(registers_t regs);

void keyboardHandler(registers_t regs);

void *systemCallHandler(int sysCallNumber, void ** args);

//Used only by getCPUspeed. makes it atomic

void disableTaskSwitch();

void enableTaskSwitch();

isr_t interruptHandlers[256];

#endif

