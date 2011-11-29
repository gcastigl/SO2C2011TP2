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

void paging_init() {
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
    while (i < placement_address+PAGE_SIZE) {
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

void _logPage(page_t page, int i, int j) {
    log(L_TRACE, "%d:%d (0x%x): f:0x%x ( %s%s%s%s) r:0x%x",
        i,
        j,
        (i<<22)|(j<<12),
        page.frame,
        page.present ? "present " : "not-present ",
        page.accessed ? "accessed " : "",
        page.dirty ? "dirty " : "",
        page.rw ? "read-write " : "",
        page.unused
    );
}

void _logTable(page_table_t *table, int i) {
    if (table != NULL) {
        for (int j = 0; j < PAGE_COUNT; ++j) {
            _logPage(table->pages[j], i, j);
        }
    } else {
        //log(L_INFO, "%d: UNMAPPED", i);
    }
}

void _logDirectory(page_directory_t *dir) {
    for (int i = 0; i < PAGE_TABLE_COUNT; ++i) {
        _logTable(dir->tables[i], i);
    }
}

void paging_enable(page_directory_t *dir) {
    current_directory = dir;
    __asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    u32int cr0;
    __asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    __asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32int address, int make, page_directory_t *dir) {
    // Turn the address into an index.
    address /= PAGE_SIZE;
    // Find the page table containing this address.
    u32int table_idx = address / PAGE_COUNT;

    if (dir->tables[table_idx]) { // If this table is already assigned
        return &dir->tables[table_idx]->pages[address%PAGE_COUNT];
    } else if(make) {
        u32int tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] = tmp | (PAGE_PRESENT | PAGE_READWRITE | PAGE_USERMODE);
        return &dir->tables[table_idx]->pages[address%PAGE_COUNT];
    } else {
        return 0;
    }
}


void page_fault(registers_t regs) {
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    u32int faulting_address;
    __asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    // The error code gives us details of what happened.
    log(L_ERROR, "PAGE-FAULT ( %s%s%s%s%s) at 0x%x",
        regs.err_code & PAGE_PRESENT    ? "page-present " : "page-not-present ",
        regs.err_code & PAGE_READWRITE  ? "write-on-read-only " : "",
        regs.err_code & PAGE_USERMODE   ? "processor-was-in-user-mode " : "",
        regs.err_code & PAGE_CORRUPTED  ? "overwritten-cpu-reserved-bits-of-page-entry " : "",
        regs.err_code & PAGE_INST_FETCH ? "instruction-fetch " : "",
        faulting_address
    );
    tty_setFormatToCurrTTY(video_getFormattedColor(RED, BLACK));
    _logPage(*get_page(faulting_address, 0, current_directory), faulting_address>>22, faulting_address>>12);
    printf("Page fault @ 0x%x! killing current process\n", faulting_address);
    panic("Page fault", 1, false);
    killCurrent();
}

PRIVATE u32int create_stack(void *new_stack_start, u32int size) {
    u32int i;
    // Allocate some space for the new stack.
    for (i = (u32int) new_stack_start; i >= ((u32int) new_stack_start - size -1);
            i -= PAGE_SIZE) {
        // General-purpose stack is in user-mode.
        alloc_frame(get_page(i, 1, current_directory), 1 /* User mode */, 1 /* Is writable */
        );
    }

    // Flush the TLB by reading and writing the page directory address again.
    u32int pd_addr;
    __asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
    __asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

    for (i = (u32int) new_stack_start; i >= ((u32int) new_stack_start - size -1);
                i -= PAGE_SIZE) {
        _logPage(*get_page(i, 0, current_directory), 0, 0);
    }
    return i+PAGE_SIZE;
}

PUBLIC int paging_dropStack(int stack_startaddr, int stacksize) {
    return -1;
}

PUBLIC int paging_reserveStack(int size) {
    static int OLD = 0x11FFF000;
    static int inc = 0x00100000;
    int a = create_stack((void*)OLD, size);
    OLD -= inc;
    return a;
}
