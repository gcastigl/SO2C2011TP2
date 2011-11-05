#include <lib/kheap.h>

#define ALIGN 		1
#define NOT_ALIGN	!ALIGN
#define MAGIC_NUMBER 300
extern int end;

int nf_address = (int) &end;


void* _malloc (int size, int align, u32int *phys) {
	void* tmp = (void*) nf_address;
	nf_address += (size + MAGIC_NUMBER);
	return (void*) tmp;
}

void kfree(void* p) {
	return; //free((void*)p, kheap);
}

void* _kmalloc(char* file, int line, int size) {
	void *ret = _malloc( size, NOT_ALIGN, NO_PHYS );
	_log(file, line, L_TRACE, "***KMALLOC*** %d", (int)ret);
	return ret;
}

