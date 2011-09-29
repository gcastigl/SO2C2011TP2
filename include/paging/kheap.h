#ifndef KHEAP_H
#define KHEAP_H

#include <defs.h>

u32int _kmalloc_p(u32int sz, u32int *phys, int align); // returns a physical address.
u32int _kmalloc(u32int sz, int align); // vanilla (normal).

#define kmalloc(sz) _kmalloc(sz, 0) // vanilla (normal).
#define kmalloc_a(sz) _kmalloc(sz, 1) // page aligned.
#define kmalloc_p(sz, phys) _kmalloc_p(sz, phys, 0) // returns a physical address.
#define kmalloc_ap(sz, phys) _kmalloc_p(sz, phys, 1) // page aligned and returns a physical address

#endif