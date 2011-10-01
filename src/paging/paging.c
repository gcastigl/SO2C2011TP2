#include <paging/paging.h>

// The kernel's page directory
page_directory_t *kernel_directory=0;

// The current page directory;
page_directory_t *current_directory=0;

extern heap_t *kheap;

// Static function to set a bit in the frames bitset
static void set_frame(u32int frame_addr) {
    u32int frame = frame_addr/0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u32int frame_addr) {
    u32int frame = frame_addr/0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}
/*
// Static function to test if a bit is set.
static u32int test_frame(u32int frame_addr) {
    u32int frame = frame_addr/0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}
*/
// Static function to find the first free frame.
static u32int first_frame() {
    u32int i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; j++) {
                u32int toTest = 0x1 << j;
                if (!(frames[i] & toTest)) {
                    return i*4*8+j;
                }
            }
        }
    }
    return ERROR;
}

static page_table_t *clone_table(page_table_t *src, u32int *physAddr) {
    page_table_t *table = (page_table_t*) kmalloc_ap(sizeof(page_table_t), physAddr);
    memset(table, 0, sizeof(page_directory_t));
    
    int i;
    for (i = 0; i < 1024; i++) {
        if (!src->pages[i].frame) {
            continue;
        }
        
        alloc_frame(&table->pages[i], 0, 0);
        if (src->pages[i].present)  table->pages[i].present = 1;
        if (src->pages[i].rw)       table->pages[i].rw = 1;
        if (src->pages[i].user)     table->pages[i].user = 1;
        if (src->pages[i].accessed) table->pages[i].accessed = 1;
        if (src->pages[i].dirty)    table->pages[i].dirty = 1;
        copy_page_physical(src->pages[i].frame * 0x1000, table->pages[i].frame * 0x1000);
    }
    
    return table;
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0) {
        return; // Frame was already allocated, return straight away.
    }
    else
    {
        u32int idx = first_frame(); // idx is now the index of the first free frame.
        if (idx == (u32int)-1) {
            //Error: PANIC("No free frames!");
            return;
        }
        set_frame(idx*0x1000); // this frame is now ours!
        page->present = 1; // Mark it as present.
        page->rw = (is_writeable)?1:0; // Should the page be writeable?
        page->user = (is_kernel)?0:1; // Should the page be user-mode?
        page->frame = idx;
    }
}

// Function to deallocate a frame.
void free_frame(page_t *page) {
    u32int frame;
    if (!(frame=page->frame)) {
        return; // The given page didn't actually have an allocated frame!
    }
    else {
        clear_frame(frame); // Frame is now free again.
        page->frame = 0x0; // Page now doesn't have a frame.
    }
}

void initialise_paging() {
    // The size of physical memory. For the moment we 
    // assume it is 16MB big.
    u32int mem_end_page = 0x1000000;
    
    nframes = mem_end_page / 0x1000;
    frames = (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    // Let's make a page directory.
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (u32int)kernel_directory->tablesPhysical;

    // Map some pages in the kernel heap area.
    // Here we call get_page but not alloc_frame. This causes page_table_t's 
    // to be created where necessary. We can't allocate frames yet because they
    // they need to be identity mapped first below, and yet we can't increase
    // placement_address between identity mapping and enabling the heap!
    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000) {
        get_page(i, 1, kernel_directory);
    }
    
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
    while (i < placement_address+0x1000)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
    
    // Now, enable paging!
    switch_page_directory(kernel_directory);

    // Initialise the kernel heap.
    kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
    
    current_directory = clone_directory(kernel_directory);
    switch_page_directory(current_directory);
}

void switch_page_directory(page_directory_t *dir) {
   current_directory = dir;
   asm volatile("mov %0, %%cr3":: "r" (dir->physicalAddr));
   u32int cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_directory_t *clone_directory(page_directory_t *src) {
    u32int phys;
    
    page_directory_t *dir = (page_directory_t*)kmalloc_ap(sizeof(page_directory_t), &phys);
    memset(dir, 0, sizeof(page_directory_t));
    
    u32int offset = (u32int) dir->tablesPhysical - (u32int) dir;
    dir->physicalAddr = phys + offset;
    
    int i;
    for (i = 0; i < 1024; ++i) {
        if (!src->tables[i]) {
            continue;
        }
        if (kernel_directory->tables[i] == src->tables[i]) {
            dir->tables[i] = src->tables[i];
            dir->tablesPhysical[i] = src->tablesPhysical[i];
        } else {
            u32int phys;
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tablesPhysical[i] = phys  | 0x07;
        }
    }
    
    return dir;
}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
   // Turn the address into an index.
   address /= 0x1000;
   // Find the page table containing this address.
   u32int table_idx = address / 1024;
   if (dir->tables[table_idx]) { // If this table is already assigned 
       return &dir->tables[table_idx]->pages[address % 1024];
   } else if(make) {
       u32int tmp;
       dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
       memset(dir->tables[table_idx], 0, 0x1000);
       dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
       return &dir->tables[table_idx]->pages[address % 1024];
   } else {
       return 0;
   }
}