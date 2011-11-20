#include <paging/frame.h>
#include <lib/string.h>
#include <util/logger.h>

u32int *frames;
u32int nframes;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) 	(a/(8*4))
#define OFFSET_FROM_BIT(a) 	(a%(8*4))

void init_frame(u32int mem_end_page) {
	nframes = mem_end_page / 0x1000;
	frames = (u32int*) kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
}

// Static function to set a bit in the frames bitset
void set_frame(u32int frame_addr) {
	log(L_DEBUG, "new frame: %d", frame_addr);
	u32int frame = frame_addr/0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
void clear_frame(u32int frame_addr) {
   u32int frame = frame_addr/0x1000;
   u32int idx = INDEX_FROM_BIT(frame);
   u32int off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
u32int test_frame(u32int frame_addr) {
   u32int frame = frame_addr/0x1000;
   u32int idx = INDEX_FROM_BIT(frame);
   u32int off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
u32int first_frame() {
   u32int i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
   {
       if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
       {
           // at least one bit is free here.
           for (j = 0; j < 32; j++)
           {
               u32int toTest = 0x1 << j;
               if ( !(frames[i]&toTest) )
               {
                   return i*4*8+j;
               }
           }
       }
   }
   return 0;
}


