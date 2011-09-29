#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <defs.h>
#include <io.h>
#include <driver/keyboard.h>
#include <main.h>
#include <command.h>

#define TT_MEDITIONS	100

void int_08();
void int_09();
void int_80(int sysCallNumber, void ** args);
#endif

