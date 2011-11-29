#ifndef STDLIB_H
#define STDLIB_H

#include <defs.h>
#include <memory/kheap.h>

void* malloc(u32int neededMem);

void free(void * pointer);

void* calloc(u32int size);

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

/* atoi
*
* Recibe como parametros:
* - String
*
* Devuelve el valor numérico del string dado
**/
int atoi(const char *str);
int htoi(const char *str);

#endif

