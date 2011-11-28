// paging.c -- Defines the interface for and structures relating to paging.
//             Written for JamesM's kernel development tutorials.

#include <memory/frame.h>
#include <memory/paging.h>
#include <memory/kheap.h>
#include <process/scheduler.h>

// The kernel's page directory
page_directory_t *kernel_directory=0;

// The current page directory;
page_directory_t *current_directory=0;

// Defined in kheap.c
extern u32int placement_address;
extern heap_t *kheap;

void paging_init()
{
    // The size of physical memory. For the moment we 
    // assume it is 16MB big.
    u32int mem_end_page = 0x1000000;
    
    nframes = mem_end_page / PAGE_SIZE;
    frames = (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    // Let's make a page directory.
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    // Map some pages in the kernel heap area.
    // Here we call get_page but not alloc_frame. This causes page_table_t's 
    // to be created where necessary. We can't allocate frames yet because they
    // they need to be identity mapped first below, and yet we can't increase
    // placement_address between identity mapping and enabling the heap!
    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
        get_page(i, 1, kernel_directory);

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    // Allocate a lil' bit extra so the kernel heap can be
    // initialised properly.
    i = 0;
    while (i < placement_address+PAGE_SIZE)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += PAGE_SIZE;
    }

    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

    // Before we enable paging, we must register our page fault handler.
    registerInterruptHandler(14, page_fault);

    // Now, enable paging!
    paging_enable(kernel_directory);

    // Initialise the kernel heap.
    kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}

void paging_enable(page_directory_t *dir)
{
    current_directory = dir;
    __asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    u32int cr0;
    __asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    __asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
    // Turn the address into an index.
    address /= PAGE_SIZE;
    // Find the page table containing this address.
    u32int table_idx = address / PAGE_COUNT;

    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%PAGE_COUNT];
    }
    else if(make)
    {
        u32int tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%PAGE_COUNT];
    }
    else
    {
        return 0;
    }
}


void page_fault(registers_t regs)
{
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    u32int faulting_address;
    __asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    // The error code gives us details of what happened.
    log(L_ERROR, "PAGE-FAULT ( %s%s%s%s%s) at 0x%x",
        regs.err_code & 0x1  ? "page-present " : "page-not-present ",
        regs.err_code & 0x2  ? "write-on-read-only " : "",
        regs.err_code & 0x4  ? "processor-was-in-user-mode " : "",
        regs.err_code & 0x8  ? "overwritten-cpu-reserved-bits-of-page-entry " : "",
        regs.err_code & 0x10 ? "instruction-fetch " : "",
        faulting_address
    );
    panic("Page fault", 1, false);
    killCurrent();
}
