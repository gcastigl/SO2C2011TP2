#ifndef _kernel_
#define _kernel_

#include <defs.h>
#include <multiboot.h>
#include <interrupts.h>
#include <driver/video.h>
#include <asm/libasm.h>
#include <defs.h>
#include <io.h>
#include <lib/stdio.h>
#include <kc.h>

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
size_t __write(int fd, const void* buffer, size_t count);

/* __read
*
* Recibe como parametros:
* - File Descriptor
* - Buffer a donde escribir
* - Cantidad
*
**/
size_t __read(int fd, void* buffer, size_t count);

#endif

