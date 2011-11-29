#ifndef FRAME_H
#define FRAME_H

#include <defs.h>
#include <memory/paging.h>

// A bitset of frames - used or free.
u32int *frames;
u32int nframes;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// function to set a bit in the frames bitset
PUBLIC void set_frame(u32int frame_addr);

// function to clear a bit in the frames bitset
PUBLIC void clear_frame(u32int frame_addr);

// function to test if a bit is set.
PUBLIC u32int test_frame(u32int frame_addr);

// function to find the first free frame.
PUBLIC u32int first_frame();

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable);

// Function to deallocate a frame.
void free_frame(page_t *page);

#endif // FRAME_H
