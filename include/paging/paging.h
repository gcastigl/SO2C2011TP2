#ifndef PAGING_H
#define PAGING_H

#include <defs.h>
#include <paging/frame.h>
#include <interrupts/interrupts.h>
#include <util/logger.h>

#define TABLE_ENTRIES	1024

typedef struct page {
   u32int present    : 1;   // Page present in memory
   u32int rw         : 1;   // Read-only if clear, readwrite if set
   u32int user       : 1;   // Supervisor level only if clear
   u32int accessed   : 1;   // Has the page been accessed since last refresh?
   u32int dirty      : 1;   // Has the page been written to since last refresh?
   u32int unused     : 7;   // Amalgamation of unused and reserved bits
   u32int frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table {
   page_t pages[1024];
} page_table_t;

typedef struct page_directory {
   page_table_t *tables[1024];
   u32int tablesPhysical[1024];
   u32int physicalAddr;
} page_directory_t;


void paging_init(u32int start);

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

/*
void alloc_frame(page_t *page, int is_kernel, int is_writeable);

void free_frame(page_t *page);
*/

#endif
