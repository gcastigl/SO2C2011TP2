#ifndef STDLIB_H
#define STDLIB_H

#include <defs.h>

/*
void* malloc(u32int neededMem);

void* calloc(u32int size);

void free(void * pointer);
*/
void memcpy(void* to, void* from, u32int count);

/* panic
*
* Recibe como parametros:
* - Mensaje
* - Línea
* - Lock
*
* Muestra el mensaje 'msg' en la línea 'line' y si lock es true, haltea
**/
void panic(char* msg, int line, int lock);

void* realloc(void* ptr, int newsize);


/* atoi
*
* Recibe como parametros:
* - String
*
* Devuelve el valor numérico del string dado
**/
int atoi(const char *str);

#endif

