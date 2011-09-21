#ifndef _kernel_
#define _kernel_

#define OS_PID	0

#include "defs.h"
#include "video.h"
#include "interrupts.h"
#include "kasm.h"
#include "defs.h"

int (*player)(void);

typedef enum eINT_80 {WRITE=0, READ} tINT_80;
typedef enum eUSER {U_KERNEL=0, U_NORMAL} tUSERS;

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

