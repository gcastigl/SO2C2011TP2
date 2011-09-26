#ifndef _kernel_
#define _kernel_

#include "defs.h"
#include "video.h"
#include "interrupts.h"
#include "kasm.h"
#include "defs.h"
#include "io.h"
#include "scheduler.h"
#include "command.h"

int getNextPid();
int getCurrPid();
int setCurrPid(int pid);
boolean noProcesses();
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

