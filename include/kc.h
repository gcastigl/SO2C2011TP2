/**********************
 kc.h
**********************/
#ifndef __KC_H__
#define __KC_H__

#include "defs.h"
#define NULL 0
#define WHITE_TXT 0x07 // Atributo de video. Letras blancas, fondo negro
int GetTTY(void);
void nice(char* buffer);

void putchar(char  c, int offset, int minioffset);
void k_clear_screen() ;
void clear_vid();
int lenspace(char*);
int itoa2(unsigned int, char*);
int atoi2(char*);
void memcpy2(char*, char*, int);
void memcpy3(char*, char*, int);
int memcmp2(char*, const char*, int);
void writechar(char c);
void mess(char* s);
void messl(char* s);
void message(char * mensaje, int offset, int minioffset);
void blank(int cant, int offset, int minioffset);
int strlen2(char*);
void Sleep(int n);
int Shell(int argc, char* argv[]);
int Idle(int argc, char* argv[]);
int Payaso(int argc, char* argv[]);
PROCESS* GetProcessByPID (int pid);
PROCESS* GetProcessByName (char* proceso);
void Destroy(int PID);
void* Malloc(int size);
void* Calloc(int size);
void kill(char* buffer);
void nice(char* buffer);
void Cleaner(void);
void SetupScheduler();


void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access, byte cero);
void setup_GDT_entry (DESCR_SEG* item, dword base, dword limit, byte access, byte attribs);
BUFFERTYPE* GetBuffer (void);
void CreateProcessAt(char*,int (*process)(int,char**),int tty, int argc, char** argv, int stacklength, int priority, int isFront);
int Tope(int argc, char* argv[]);
int DancingMessage(int argc,char* argv[]);
void Scroll(void);
int Payaso(int argc, char* argv[]);
#endif
