#ifndef PAGING_H
#define PAGING_H

#include <defs.h>
#include <util/logger.h>

#define TABLE_ENTRIES	1024

void paging_init(u32int start);

#endif
