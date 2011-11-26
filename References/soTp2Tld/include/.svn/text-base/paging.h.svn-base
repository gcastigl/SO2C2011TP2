#ifndef _PAGING_H
#define _PAGING_H

#include "exceptions.h"
#include "stdio.h"
#include "kheap.h"

#define MAKE 1
#define ENABLE 1
#define DISABLE !ENABLE

// Macros usadas en los algoritmos de control de frames en uso o libres
#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

// Estructura de pagina

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
} page_dir_t;



// funcion que crea la tabla de directorios y de paginas asi como habilita la paginacion
void initPaging( void );

// habilita la paginacion ubicando en el registro CR3 la direccion FISICA de la tabla de directorios
// ademas de encender el bit PG del registro CR0, con lo cual se habilita la paginacion
void enablePaging( page_dir_t *directoryTable );

// Dada una direccion de memoria virtual retorna el puntero a la pagina donde deberia estar
// en la tabla de paginas correspondiente
// si make == 1 y la pagina aun no existe, la aloca en memoria
page_t * getPage( int address, int make, page_dir_t * dir );

// Setea como no presentes todas las paginas que contienen el stack de un proceso dado como parametro,
// ademas de las paginas de todos sus ancestros
void downPages	( int process );

// Setea como presentes todas las paginas que contienen el stack de un proceso dado como parametro,
// ademas de las paginas de todos sus ancestros
void upPages	( int process );

#endif
