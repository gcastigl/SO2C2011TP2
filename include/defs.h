/***************************************************
  Defs.h
	
****************************************************/

#ifndef _defs_
#define _defs_

#define byte unsigned char
#define word short int
#define dword int
#define CANTTERM 8

#define MAX_PROCESS_NAME 20
#define KERNEL_MALLOC_SIZE 4
#define SHIFT_PRESSED 1
#define CONTROL_PRESSED 2
#define ALT_PRESSED 4
#define PRIORITY_RATIO 2
#define MAX_PRIORITY 4
/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT		0x80			/* segmento presente en memoria */
#define ACS_CSEG		0x18			/* segmento de codigo */
#define ACS_DSEG		0x10			/* segmento de datos */
#define ACS_READ		0x02			/* segmento de lectura */
#define ACS_WRITE		0x02			/* segmento de escritura */
#define ACS_IDT			ACS_DSEG
#define ACS_INT_386		0x0E			/* Interrupt GATE 32 bits */
#define ACS_INT			( ACS_PRESENT | ACS_INT_386 )
#define ACS_PRIV0		0x00
#define ACS_PRIV1		0x20
#define ACS_PRIV2		0x40
#define ACS_PRIV3		0x60
#define ACS_CODE		(ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA		(ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK		(ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#pragma pack (1)		/* Alinear las siguiente estructuras a 1 byte */

typedef enum { FALSE, TRUE} boolean;

/* Descriptor de segmento */
typedef struct
{
	word limit;
	word base_l;
	byte base_m;
	byte access;
	byte attribs;
	byte base_h;
} DESCR_SEG;

/* Descriptor de interrupcion */
typedef struct
{
	word offset_l;
	word selector;
	byte cero;
	byte access;
	word offset_h;
} DESCR_INT;

/* IDTR  */
typedef struct
{
	word limit;
	dword base;
} IDTR;

/* GDTR */     
typedef struct 
{
	word limit;
	dword base;
} GDTR;

typedef struct 
{
	int buffer[10];
	int head;
	int tail;
} BUFFERTYPE;

typedef struct
{
	char* terminal;
	int movimiento;
	BUFFERTYPE buffer;
} TTY;




typedef struct PROCESS* pPROCESS;

typedef struct PROCESS
{
	int pid;
	char name [MAX_PROCESS_NAME];
	int priority;
	int tty;
	int foreground;
	int lastCalled;
	int sleep;
	int blocked;
	int parent;
	int ESP;
	int free;
	int stackstart;
	int stacksize;

} PROCESS;

typedef struct
{
	int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,  EIP, CS, EFLAGS;
	void*retaddr;
	int argc;
	char** argv;
} STACK_FRAME;




#endif
