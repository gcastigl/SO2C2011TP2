
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "video.h"

#define MAX_CONSOLES 5
#define MAX_LINE 320  		// Tamano maximo de una linea  ( 4 lineas de comando )


typedef struct console{

  char 	tty;			// numero que identifica las consolas;
  char 	vbuffer[4000];		// COL*ROWS*2 buffer de video 80*25*2

  char 	kbuffer[MAX_LINE];	// buffer de teclado
  int 	kcur;			// longitud de comando
  int 	kfull;			// buffer de teclado lleno

  unsigned int	cursor;  		// estado del cursor durante la ejecucion
  int 	headcur;		// header cursor
  int 	sheadcur;		// sub head color
  int 	footcur;		// foot cursor
  int 	ttycur;			// date cursor
  int	currentColor;	// color actual en consola

} console_t;


void setupConsoles(void);


//Retorna el numero de tty actual
int getProcessTty(void);

//Retorna el puntero a la consola actual
console_t * getProcessConsole(void);
#endif
