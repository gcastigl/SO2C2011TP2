#ifndef IO_H
#define IO_H

#include <defs.h>
#include <driver/keyboard.h>
#include <driver/video.h>
#include <tty.h>
#include <lib/file.h>


/* sysRead
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
* Lee del file descriptor y vuelca lo leido en el buffer
**/
void sysRead(int fd, void * buffer, u32int count);

/* sysWrite
*
* Recibe como parametros:
* - File Descriptor
* - Buffer del source
* - Cantidad
*
* Escribe al file descriptor el contenido del buffer hasta count caracteres
**/
void sysWrite(int fd, void * buffer, u32int count);

/* sysOpen
*
* Recibe como parametros:
* - File Name
* - Open flags
* - Create flags
*
* Abre el archivo indicado y devuelve un file descriptor
**/
int sysOpen(char* fileName, int oflags, int cflags);

int sysClose(char* fileName, int oflags, int cflags);

#endif

