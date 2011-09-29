#ifndef _kernel_
#define _kernel_

#include <defs.h>
#include <multiboot.h>
#include <interrupts/interrupts.h>
#include <driver/video.h>
#include <asm/libasm.h>
#include <defs.h>
#include <io.h>
#include <lib/stdio.h>
#include <interrupts/interruptDefs.h>

int getNextPID();

int getCurrPID();
/* __write
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
**/
u32int __write(int fd, const void* buffer, u32int count);

/* __read
*
* Recibe como parametros:
* - File Descriptor
* - Buffer a donde escribir
* - Cantidad
*
**/
u32int __read(int fd, void* buffer, u32int count);

#endif

