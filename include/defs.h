#ifndef DEFS_H
#define DEFS_H

#define PUBLIC 
#define PRIVATE static

#define byte unsigned char
#define word short int
#define dword int
#define ushort unsigned short

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

typedef enum { false = 0, true } boolean;

#define NULL	((void *)0)
#define ASSERT(b) 

#define ERROR -1

typedef unsigned int    u32int;
typedef          int    s32int;
typedef unsigned short  u16int;
typedef          short  s16int;
typedef unsigned char   u8int;
typedef          char   s8int;
typedef void*           type_t;


#define SYSTEM_READ		3
#define SYSTEM_WRITE	4

#define STD_IN		0
#define STD_OUT		1
#define STD_ERROR	2

#define K_BUFFER_SIZE	10

// ERROR CODES
#define	E_DIR_EXISTS		1
#define E_DIR_FULL			2
#define	E_FILE_EXISTS		3
#define E_OUT_OF_MEMORY		4
#define E_CORRUPTED_FILE	5
#define E_INVALID_ARG		6

// FIXME: this should not be here!!
typedef struct page
{
  int present	: 1; //	Pagina presente en memoria
  int rw	: 1; // Read-Only si esta apagado o bien Read-Write 
  int user	: 1; // si esta apagado nivel de Supervisor
  int accessed	: 1; // indica si la pagina ha sido accedida desde el ultimo refresco 
  int dirty	: 1; // indica si la pagina ha sido escrita desde el ultimo refresco 
  int unused	: 7; // bits reservados
  int frame	: 20;// Frame Address ( corridos 12 bits hacia la derecha )
} page_t;

// Estructura de tabla de paginas

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

// Estructura de pagina de directorios

typedef struct page_dir
{
    page_table_t *tables[1024]; // punteros a las tablas de paginas
    int tablesPhysical[1024];   // direcciones físicas de las tablas de paginas
} page_directory_t;

int errno;

#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

#endif

