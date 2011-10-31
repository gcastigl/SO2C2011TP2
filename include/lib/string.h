#ifndef STRING_H
#define STRING_H

#include <defs.h>
#include <lib/stdlib.h>

/* strlen
*
* Recibe como parametros:
* - String
*
* Devuelve el largo del string
**/
u32int strlen(char *string);

/* strcmp
*
* Recibe como parametros:
* - String
* - String
*
* Compara los 2 strings
**/
int strcmp(const char * s1, const char * s2);

/* strncmp
*
* Recibe como parametros:
* - String
* - String
* - Cantidad
*
* Compara los 2 strings hasta el caracter 'n'
**/
int strncmp(char * s1, char * s2, unsigned int n);

/* substr
*
* Recibe como parametros:
* - String
* - String
*
* Chequea si s1 es substring de s2
**/
int substr(const char * s1, const char *s2);

/* strcpy
*
* Recibe como parametros:
* - String
* - String
*
* Copia el contenido de 'from' a 'to'
**/
void strcpy(char* to, char* from);

/* memset
*
* Recibe como parametros:
* - Puntero al inicio del contenido
* - Valor al cual setear la memoria
* - Cantidad de valores a setear
*
* Setea la memoria al valor indicado
**/
void *memset(void *s, int c, u32int n);

/* strncpy
*
* Recibe como parametros:
* - String
* - String
* - Tamaño
*
* Copia 'size' caracteres de 'from' a 'to'
**/
void strncpy(char *to, const char *from, int size);

/* strIndexOf
*
* Recibe como parametros:
* - String
* - Caracter
* - Offset
*
* Devuelve el índice del caracter 'c' en el string 'str'
**/
int strIndexOf(char* str, char c, int startIndex);

/* strContains
*
* Recibe como parametros:
* - String
* - Caracter
*
* Chequea si el string 'str' contiene el caracter 'c'
**/
int strContains(char* str, char c);

int strspn(const char *string, const char *in);
char *strpbrk(register const char *string, register const char *brk);
char *strtok(register char *string, const char *separators);

#endif
