
#ifndef _KHEAP_H
#define _KHEAP_H

#include <defs.h>
#include <util/logger.h>

#define KHEAP_START         0x40000000 // 32 Mb de memoria virtual - 16 Mb memoria fisica
#define HEAP_MAX_SIZE     	0x4FFFF000	
#define KHEAP_INITIAL_SIZE	0x800000 	// 8 Mb iniciales de HEAP
#define HEAP_INDEX_SIZE		0x20000		// 128 Kb
#define HEAP_MAGIC        	0x123890AB	// Codigo interno para verificar la integridad de los bloques de memoria
#define HEAP_MIN_SIZE     	0x70000		

#define PAGE_ALIGN 	1
#define NOT_ALIGN	!ALIGN
#define NO_PHYS		0


void kfree	( void* p );		//   General deallocation function.

void*  _kmalloc	(char* file, int line, int size );		//   General allocation function.
#define kmalloc(...) _kmalloc(__FILE__, __LINE__, __VA_ARGS__);

#endif
