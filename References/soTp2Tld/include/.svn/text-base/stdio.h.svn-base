#ifndef _STDIO_H
#define _STDIO_H

#include "libc.h"
#include "video.h"

#define MAX_BUS 256
#define MAX_DEV	32
#define MAX_FUN 8


#define NULL ""

void putc( char c );
void 	puts(char *s);

void 	printHex_int( int number );
void 	printHex(unsigned char dByte);

void 	printDec_int( int number );
void 	printDec(unsigned char dByte);

int 	strlen(char *s);
void 	getline(char *comand);
int     isCharIn(char key);
void 	strcpy(char *arg1,char *arg2);
int 	strcmp (char *arg1,char*arg2);
void 	memset( void * buffer, char with, int to );

void 	lspci( void );
int 	getPciData (int bus, int dev,int fun,int reg);
void 	getPciProduct(unsigned int pciLine,char **venStr,char **devStr);

int atoi(char * str);

#endif
