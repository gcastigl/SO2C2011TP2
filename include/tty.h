#ifndef TTY_H
#define TTY_H

#include <defs.h>
#include <lib/kheap.h>
#include <fs/fs.h>
#include <lib/stdlib.h>
#include <driver/video.h>
#include <process/process.h>

#define MAX_TTYs		8

#define BUFFER_SIZE	512

typedef struct {
    int id;
    int pid;
	char* terminal;
	int offset;
	char buffer[BUFFER_SIZE];
    int bufferOffset;
	u32int currDirectory;
	char currPath[64];
	int currPathOffset;
	// FIXME: This values could be wrappeed up in a VideoAttributtes structure
	char bgColor;
	char fgColor;
} TTY;

/* initTTY
*
* Inicializa una TTY
**/
int initTTY();

/* tty_setCurrent
*
* Recibe como parametros:
* - TTY
*
* Setea la TTY actual
**/
void tty_setCurrent(int tty);

/* tty_getCurrent
*
* Devuelve el número de TTY actual
**/
int tty_getCurrent();

/* tty_getCurrentTTY
*
* Devuelve la TTY actual
**/
TTY* tty_getCurrentTTY();

void tty_getCurrentNode(fs_node_t* node);

/* tty_getTTY
*
* Recibe como parametros:
* - Índice
*
* Devuelve la TTY 'index'
**/
TTY* tty_getTTY(int index);

/* tty_write
*
* Recibe como parametros:
* - TTY
* - Buffer
* - Cantidad
*
* Escribe en la tty 'tty' el contenido del buffer hasta 'size' caracteres
**/
void tty_write(TTY* tty, char* buffer, u32int size);

/* tty_clean
*
* Recibe como parametros:
* - TTY
*
* Límpia la pantalla de la tty
**/
void tty_clean(TTY* tty);

/* tty_setFormatToCurrTTY
*
* Recibe como parametros:
* - format
*
* Setea el formato a la TTY actual
**/
void tty_setFormatToCurrTTY(char format);

/* tty_getFormatToCurrTTY
*
* Devuelve el formato de la TTy actual
**/
char tty_getCurrTTYFormat();


/* startTTYs
*
* Inicializa las TTYs
**/
void startTTYs();

/* tty_p
*
* Recibe como parametros:
* - Cantidad de argumentos
* - Argumentos
*
* Proceso TTY
**/
int tty_p(int argc, char **argv);

#endif
