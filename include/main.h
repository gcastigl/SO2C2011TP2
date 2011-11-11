#ifndef MAIN_H
#define MAIN_H

#include <defs.h>
#include <multiboot.h>
#include <asm/libasm.h>
#include <interrupts/interrupts.h>
#include <fs/fs.h>
#include <paging/paging.h>

/* getNextPID
*
* Devuelve el próximo PID libre
**/
int getNextPID();

#endif

