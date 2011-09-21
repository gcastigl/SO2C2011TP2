#ifndef _defs_
#define _defs_

#define byte unsigned char
#define word short int
#define dword int
#define ushort unsigned short
typedef unsigned long long int uint64_t;

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT     0x80            /* segmento presente en memoria */
#define ACS_CSEG        0x18            /* segmento de codigo */
#define ACS_DSEG        0x10            /* segmento de datos */
#define ACS_READ        0x02            /* segmento de lectura */
#define ACS_WRITE       0x02            /* segmento de escritura */
#define ACS_IDT         ACS_DSEG
#define ACS_INT_386 	0x0E		/* Interrupt GATE 32 bits */
#define ACS_INT         ( ACS_PRESENT | ACS_INT_386 )

/* Interrupt Masks */
#define NONE	0xFF
#define INT_08	0xFE
#define INT_09	0xFD
#define INT_0A	0xFB
#define INT_0B	0xF7
#define INT_0C	0xEF
#define INT_0D	0xDF
#define INT_0E	0xBF
#define INT_0F	0x7F
#define INT_80	0x7E

#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define NULL	((void *)0)
#define TRUE	1
#define FALSE	0

#define SYSTEM_READ		3
#define SYSTEM_WRITE	4
/*
0	STD	NOHIDE	{ int nosys(void); } syscall nosys_args int
1	STD	NOHIDE	{ void exit(int rval); } exit rexit_args void
2	STD	POSIX	{ int fork(void); }
3	STD	POSIX	{ ssize_t read(int fd, void *buf, size_t nbyte); }
4	STD	POSIX	{ ssize_t write(int fd, const void *buf, size_t nbyte); }
5	STD	POSIX	{ int open(char *path, int flags, int mode); }
6	STD	POSIX	{ int close(int fd); }
*/
#define STD_IN		0
#define STD_OUT		1
#define STD_ERROR	2

#define K_BUFFER_SIZE	10

#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

typedef unsigned int size_t;
typedef short int ssize_t;

/* Descriptor de segmento */
typedef struct {
	word	limit,
	base_l;
	byte	base_m,
	access,
	attribs,
	base_h;
} DESCR_SEG;


/* Descriptor de interrupcion */
typedef struct {
	word	offset_l,
			selector;
	byte	cero,
			access;
	word	offset_h;
} DESCR_INT;

/* IDTR  */
typedef struct {
  word  limit;
  dword base;
} IDTR;

#endif

