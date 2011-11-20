#ifndef FRAME_H_
#define FRAME_H_

#include <defs.h>

void init_frame(u32int mem_end_page);

void set_frame(u32int frame_addr);

void clear_frame(u32int frame_addr);

u32int test_frame(u32int frame_addr);

u32int first_frame();

#endif /* FRAME_H_ */
