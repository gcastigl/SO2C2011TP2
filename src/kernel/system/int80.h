#include "isr.h"

#ifndef INT80_H
#define	INT80_H

typedef void (*int80_t)(registers_t);
void register_functionality(uint8_t n, int80_t func);
void init_int80();

#endif	/* INT80_H */
