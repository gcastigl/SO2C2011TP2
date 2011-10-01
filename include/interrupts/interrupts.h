#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <defs.h>
#include <io.h>
#include <driver/keyboard.h>

#define TT_MEDITIONS	100

void int_08();
void int_09();
void int_80(int sysCallNumber, void ** args);
void bounds(void);
void invop(void);
void div0(void);
void gpf(void);
void ssf(void);
void snp(void);
void pageFault(void);
#endif

