#ifndef _PAGING_H
#define _PAGING_H

#include <defs.h>
#include <lib/kheap.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <process/process.h>

#define MAKE 1
#define ENABLE 1
#define DISABLE !ENABLE

// Macros usadas en los algoritmos de control de frames en uso o libres
#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

// funcion que crea la tabla de directorios y de paginas asi como habilita la paginacion
void initialize_paging( void );

// habilita la paginacion ubicando en el registro CR3 la direccion FISICA de la tabla de directorios
// ademas de encender el bit PG del registro CR0, con lo cual se habilita la paginacion
void enablePaging( page_directory_t *directoryTable );

// Dada una direccion de memoria virtual retorna el puntero a la pagina donde deberia estar
// en la tabla de paginas correspondiente
// si make == 1 y la pagina aun no existe, la aloca en memoria
page_t * getPage( int address, int make, page_directory_t * dir );

// Setea como no presentes todas las paginas que contienen el stack de un proceso dado como parametro,
// ademas de las paginas de todos sus ancestros
void downPages	( int process );

// Setea como presentes todas las paginas que contienen el stack de un proceso dado como parametro,
// ademas de las paginas de todos sus ancestros
void upPages	( int process );

void alloc_frame( page_t *page, int is_kernel, int is_writeable );
void free_frame( page_t *page );
page_t *get_page( int address, int make, page_directory_t *dir );

#endif
