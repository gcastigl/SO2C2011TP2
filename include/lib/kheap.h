
#ifndef _KHEAP_H
#define _KHEAP_H

#include <defs.h>
#include <lib/ordered_map.h>
#include <paging/paging.h>

#define KHEAP_START         	0x40000000 // 32 Mb de memoria virtual - 16 Mb memoria fisica
#define HEAP_MAX_SIZE     	0x4FFFF000	
#define KHEAP_INITIAL_SIZE	0x800000 	// 8 Mb iniciales de HEAP
#define HEAP_INDEX_SIZE		0x20000		// 128 Kb
#define HEAP_MAGIC        	0x123890AB	// Codigo interno para verificar la integridad de los bloques de memoria
#define HEAP_MIN_SIZE     	0x70000		

#define PAGE_ALIGN 	1
#define NOT_ALIGN	!ALIGN
#define NO_PHYS		0


/**    ESTRUCTURAS PARA EL MANEJO DE MEMORIA  ARRAY ORDENADO **/

typedef struct
{
    int magic;   // Magic number, used for error checking and identification.
    int is_hole;   // 1 if this is a hole. 0 if this is a block.
    int size;    // size of the block, including the end footer.
} header_t;

typedef struct
{
    int magic;     	// Magic number, same as in header_t.
    header_t *header; 	// Pointer to the block header.
} footer_t;

typedef struct
{
    ordered_array_t index;
    int start_address; // The start of our allocated space.
    int end_address;   // The end of our allocated space. May be expanded up to max_address.
    int max_address;   // The maximum address the heap can be expanded to.
    int supervisor;     // Should extra pages requested by us be mapped as supervisor-only?
    int readonly;       // Should extra pages requested by us be mapped as read-only?
} heap_t;


void*  kmalloc	( int size );		//   General allocation function. 
void kfree	( void* p );		//   General deallocation function.

void free 	( void *p, heap_t *heap); 		// Releases a block allocated with 'alloc'.
void* kmalloc_a	( int size );			  	// malloc con alineacion a pagina de 4KB
void* kmalloc_p	( int size, u32int *phys );		// malloc con retorno de direccion fisica
void* kmalloc_ap	( int size, u32int *phys ); 		// malloc con alineacion a pagina & retorno de direccion fisica
void* alloc	( int size, int page_align, heap_t *heap); 

/* Allocates a contiguous region of memory 'size' in size. If page_align==1, it creates that block starting
   on a page boundary. */

heap_t *create_heap(int start, int end, int max, int supervisor, int readonly);	//Create a new heap.

#endif
