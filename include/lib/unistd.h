#ifndef UNINSTD_H_
#define UNINSTD_H_

#include <defs.h>
#include <asm/libasm.h>
#include <stdarg.h>
#include <fs/file.h>
#include <fs/fs_API.h>

int open(const char *path, int oflag, ...);

int creat(const char *path, int mode);	// idem as open

int close(int fildes);

/* write
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
**/
u32int write(int fd, const void* buffer, u32int count);

/* read
*
* Recibe como parametros:
* - File Descriptor
* - Buffer a donde escribir
* - Cantidad
*
**/
u32int read(int fd, void* buffer, u32int count);

int lseek(int fildes, int offset, int oflag);

/* mkfifo
*
* Recibe como parametros:
* - Nombre
* - Modo
*
* Crea un named pipe en el directorio actual
**/
int mkfifo(char *name, int mode);
#endif
