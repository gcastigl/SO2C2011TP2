#include "../../include/varargs.h"
#include "../../include/stdarg.h"

#ifndef STDIO_H
#define STDIO_H

char getchar();
void putchar(char c);
void printf( char * formatString, ...);
int sscanf(char *formatString, char *format, ...);

#endif //STDIO_H
