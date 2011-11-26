#ifndef _LIBC_H
#define _LIBC_H

#include "defs.h"
#include "stdio.h"
#include "shell.h"
#include "video.h"
//#include "console.h"
#include"process.h"


#define VIDEO 0xb8000	   	// una linea mas abajo de de 0xB8000

#define VIDEO_FOOT 0xB8F00 	// Pie de pantalla (1 linea)
#define VIDEO_HEAD 0xB8000	// Encabezado (1 linea)

#define OFF 0
#define ON !OFF

/*Numero de teclas del teclado*/
#define NUM_KEYS 90

/*Special Keys*/
#define BACK_SPACE '\b'
#define TAB	0xC2
#define TAB_STOP 8
#define ERROR 'e'
#define ESC 0xF1
#define ENTER '\n'
#define CTRL 0xE0  
#define L_SHIFT 's'
#define R_SHIFT 's'
#define PT_SCROLL '*'
#define ALT 0xE1 
#define SPACE ' '
#define CPSLK '+' 
#define  F1 0x91
#define  F2 0x91
#define  F3 0x91
#define  F4 0x91
#define  F5 0x91
#define  F6 0x91
#define  F7 0x91
#define  F8 0x91
#define  F9 0x91
#define  F10 0x91
#define F11 0x91
#define F12 0x91
#define NOPRINT '+'
#define NUM_LK 0xE6
#define SCRL_LK '+'
#define HOME '+'
#define UP_ARROW '+'	
#define PG_UP '+'
#define DEL '+'
 
/*MakeCodes and Breakcodes SHIFT Y CAPSLOCK*/
#define MKC_RSHIFT 0x36 
#define BRK_RSHIFT 0xB6
#define MKC_LSHIFT 0x2A 
#define BRK_LSHIFT 0xAA
#define MKC_CAPS 0x3A

/*Scancodes */
#define SCODE_Q 0x10
#define SCODE_P 0x19
#define SCODE_A 0x1E
#define SCODE_L 0x26
#define SCODE_Z 0x2C
#define SCODE_M 0x32
#define SCODE_ACENTO 0x1A
#define SCODE_E 0x12
#define SCODE_I 0x17
#define SCODE_O 0x18
#define SCODE_U 0x16
#define SCODE_SPKEY 0xE0
#define SCODE_DEL 0x53
#define SCODE_DOWNARR 0x50
#define SCODE_END 0x4F
#define SCODE_HOME 0x47
#define SCODE_INS 0x52
#define SCODE_LEFTARR 0x4B
#define SCODE_PGDN 0x51
#define SCODE_PGUP 0x49
#define SCODE_RIGHTARR 0x4D
#define SCODE_UPARR 0x48	
#define SCODE_RIGHTALT 0x38
#define SCODE_RIGHTCTRL 0x1D
#define SCODE_COMILLA 0x0C
#define SCODE_CIRC 0x29
#define SCODE_MAS 0x1B
#define SCODE_LLAVEABRE 0x28
#define SCODE_LLAVECIERRA 0x2B
#define SCODE_BAR 0x35
#define SCODE_ENTER  0x1C
#define SCODE_F1  0x3B
#define SCODE_F5  0x3F
/*Ascii Extendido*/
#define ENIEMAYUS 0xA5 
#define ENIEMINUS 0xA4
#define A_ACENT 0xA0
#define a_ACENT 0xA0
#define E_ACENT 0x90
#define e_ACENT 0x82
#define I_ACENT 0xA1
#define i_ACENT 0xA1
#define O_ACENT 0xA2
#define o_ACENT 0xA2
#define U_ACENT 0xA3
#define u_ACENT 0xA3
#define A_DIERE 0x8E
#define a_DIERE 0x84 
#define E_DIERE 0x89
#define e_DIERE 0x89
#define I_DIERE 0x8B
#define i_DIERE 0x8B
#define O_DIERE 0x99
#define o_DIERE 0x94
#define U_DIERE 0x9A
#define u_DIERE 0x81 
#define BARRA 0xAA
#define SIG_PREG 0xA8
#define SIG_EXC 0xAD
#define O_ 0xF8
#define SIMB1 0xF7
#define SIMB2 0xF1
#define SIMB3 0xE1
#define ACENTO 'C'

/** PUERTOS DE ENTRADA SALIDA **/

#define KWD_AD 		0x60
#define PIC_AD 		0x20
#define PCI_INDEX 	0x0CF8	
#define PCI_DATA  	0x0CFC
#define KWD_CUR_1 	0x3D4
#define KWD_CUR_2 	0x3D5
#define RTC_INDEX	0x70	// Real Time Clock
#define RTC_DATA	0x71

enum {STDOUT, KWD, PCI_D,PCI_I,USB,PIC,K_CUR_1,K_CUR_2,FOOT_TIME}; //file descriptors
enum {WRITE,READ} ;  //system calls
enum {SPANISH,ENGLISH}; //idiomas del teclado

/* Inicializa la entrada del IDT */
void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
			 byte cero);

void int_80(int systemCall,int fd,char *buffer,int count);
void int_09(void);
//void int_85(void *param); 
void int_85 (int code, void *param1, void *param2, void *param3);

void guardaBuffer(char Ascii);
int isAlpha_(unsigned char  sCode);
int isVowel_(unsigned char sCode);
char vocalAcentuada(unsigned char sCode);
char vocalDieresis (unsigned char sCode);
char convertSpKeys(unsigned char sCode);
char convertAltGr(unsigned char sCode);
char convertScode(unsigned char sCode);


int language;

int bufferLleno; //flag que indica cuando se finaliza de escribir un comando y debe leerse

#endif

