#ifndef PAGING_H
#define PAGING_H

#include <defs.h>
#include <lib/kheap.h>
#include <lib/string.h>
#include <asm/libasm.h>

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void initialise_paging();

/**
  Causes the specified page directory to be loaded into the
  CR3 register.
**/
void switch_page_directory(page_directory_t *new);

/**
  Retrieves a pointer to the page required.
  If make == 1, if the page-table in which this page should
  reside isn't created, create it!
**/
page_t *get_page(u32int address, int make, page_directory_t *dir);

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);
page_directory_t *clone_directory(page_directory_t *src);

// A bitset of frames - used or free.
u32int *frames;
u32int nframes;

// Defined in kheap.c
extern u32int placement_address;
#endif