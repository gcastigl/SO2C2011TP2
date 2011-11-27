// paging.h -- Defines the interface for and structures relating to paging.
//             Written for JamesM's kernel development tutorials.

#ifndef PAGING_H
#define PAGING_H

#include <defs.h>
#include <interrupts/interrupts.h>

#define PAGE_SIZE  0x1000
#define PAGE_COUNT 1024

typedef struct page
{
    u32int present    : 1;   // Page present in memory
    u32int rw         : 1;   // Read-only if clear, readwrite if set
    u32int user       : 1;   // Supervisor level only if clear
    u32int accessed   : 1;   // Has the page been accessed since last refresh?
    u32int dirty      : 1;   // Has the page been written to since last refresh?
    u32int unused     : 7;   // Amalgamation of unused and reserved bits
    u32int frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
    page_t pages[PAGE_COUNT];
} page_table_t;

typedef struct page_directory
{
    /**
       Array of pointers to pagetables.
    **/
    page_table_t *tables[PAGE_COUNT];
    /**
       Array of pointers to the pagetables above, but gives their *physical*
       location, for loading into the CR3 register.
    **/
    u32int tablesPhysical[PAGE_COUNT];

    /**
       The physical address of tablesPhysical. This comes into play
       when we get our kernel heap allocated and the directory
       may be in a different location in virtual memory.
    **/
    u32int physicalAddr;
} page_directory_t;

/**
   Sets up the environment, page directories etc and
   enables paging.
**/
void paging_init();

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

/**
   Handler for page faults.
**/
void page_fault(registers_t regs);

#endif
