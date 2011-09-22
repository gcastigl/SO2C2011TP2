#include "../system/int80.h"

#ifndef TIMER_H
#define	TIMER_H

void register_tick_subhandler(int80_t func); 

void init_timer_tick();


void start_ticks();
void stop_ticks();
int get_ticks();
#endif	/* TIMER_H */

