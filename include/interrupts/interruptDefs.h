#ifndef INTERRUPT_DEFS_H
#define INTERRUPT_DEFS_H

#include <defs.h>
#include <asm/libasm.h>
#include <lib/string.h>

/* Muestra la imagen de inicio */
void showSplashScreen();

/* Tiempo de espera */
void wait(int time);

/* Descriptor de segmento */
struct gdt_entry_struct
{
    u16int  limit_low;           // The lower 16 bits of the limit.
    u16int  base_low;            // The lower 16 bits of the base.
    u8int   base_middle;         // The next 8 bits of the base.
    u8int   access;              // Access flags, determine what ring this segment can be used in.
    u8int   granularity;
    u8int   base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
    u16int  limit;               // The upper 16 bits of all selector limits.
    u32int  base;                // The address of the first gdt_entry_t struct.
} __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

// A struct describing an interrupt gate.
struct idt_entry_struct
{
   u16int base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   u16int sel;                 // Kernel segment selector.
   u8int  always0;             // This must always be zero.
   u8int  flags;               // More flags. See documentation.
   u16int base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idt_ptr_struct
{
   u16int limit;
   u32int base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// Lets us access our ASM functions from our C code.
extern void _gdt_flush(u32int);
extern void _idt_flush(u32int);
void init_descriptor_tables();

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;
#endif
