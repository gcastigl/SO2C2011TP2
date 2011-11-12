#include <lib/kheap.h>


// end is defined in the linker script.
extern u32int end;
u32int placement_address = (u32int)&end;

u32int kmalloc_int(u32int sz, int align, u32int *phys)
{
    // This will eventually call malloc() on the kernel heap.
    // For now, though, we just assign memory at placement_address
    // and increment it by sz. Even when we've coded our kernel
    // heap, this will be useful for use before the heap is initialised.
    if (align == 1 && (placement_address & 0xFFFFF000) )
    {
        // Align the placement address;
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys)
    {
        *phys = placement_address;
    }
    u32int tmp = placement_address;
    placement_address += sz;
    return tmp;
}

// kmalloc page-aligned
u32int kmalloc_a(u32int sz)
{
    return kmalloc_int(sz, 1, 0);
}

// kmalloc returns a physical address
u32int kmalloc_p(u32int sz, u32int *phys)
{
    return kmalloc_int(sz, 0, phys);
}

// kmalloc page-aligned and returns a physical address
u32int kmalloc_ap(u32int sz, u32int *phys)
{
    return kmalloc_int(sz, 1, phys);
}

void* kmalloc(u32int sz)
{
    return (void*) kmalloc_int(sz, 0, 0);
}

void kfree(void* p) {

}
