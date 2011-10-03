#include <interrupts/interrupts.h>

#define SET_ISR(n) (idt_set_gate(n, (u32int)&isr##_##n, 0x08, ACS_INT))
#define SET_IRQ(n) (idt_set_gate((n+32), (u32int)&irq##_##n, 0x08, ACS_INT))
// Internal function prototypes.
static void init_gdt();
static void gdt_set_gate(s32int,u32int,u32int,u8int,u8int);
static void init_idt();
static void idt_set_gate(u8int,u32int,u16int,u8int);

void registerInterruptHandler(u8int number, isr_t handler) {
    interruptHandlers[number] = handler;
}

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables() {
   // Initialise the global descriptor table.
   init_gdt();
   init_idt();
   initBasicHandlers();
}

void initBasicHandlers() {
    memset(&interruptHandlers, 0, 256 * sizeof(isr_t));
    registerInterruptHandler(IRQ0, &timerTickHandler);
    registerInterruptHandler(IRQ1, &keyboardHandler);
}

static void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (u32int)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    _gdt_flush((u32int)&gdt_ptr);
}

// Set the value of one GDT entry.
static void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access, u8int gran) {
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
}

static void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (u32int)&idt_entries;
    
    u8int pic1mask, pic2mask;
    pic1mask = _port_in(0x21);
    pic2mask = _port_in(0xA1);
    
    // Remapping the irq table.
    _port_out(0x20, 0x11);
    _port_out(0xA0, 0x11);
    _port_out(0x21, 0x20);
    _port_out(0xA1, 0x28);
    _port_out(0x21, 0x04);
    _port_out(0xA1, 0x02);
    _port_out(0x21, 0x01);
    _port_out(0xA1, 0x01);
    _port_out(0x21, pic1mask);
    _port_out(0xA1, pic2mask);
    
    memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);
    
    SET_ISR(0);     SET_ISR(1);     SET_ISR(2);     SET_ISR(3);
    SET_ISR(4);     SET_ISR(5);     SET_ISR(6);     SET_ISR(7);
    SET_ISR(8);     SET_ISR(9);     SET_ISR(10);    SET_ISR(11);
    SET_ISR(12);    SET_ISR(13);    SET_ISR(14);    SET_ISR(15);
    SET_ISR(16);    SET_ISR(17);    SET_ISR(18);    SET_ISR(19);
    SET_ISR(20);    SET_ISR(21);    SET_ISR(22);    SET_ISR(23);
    SET_ISR(24);    SET_ISR(25);    SET_ISR(26);    SET_ISR(27);
    SET_ISR(28);    SET_ISR(29);    SET_ISR(30);    SET_ISR(31);
    
    SET_IRQ(0);     SET_IRQ(1);     SET_IRQ(2);     SET_IRQ(3);
    SET_IRQ(4);     SET_IRQ(5);     SET_IRQ(6);     SET_IRQ(7);
    SET_IRQ(8);     SET_IRQ(9);     SET_IRQ(10);    SET_IRQ(11);
    SET_IRQ(12);    SET_IRQ(13);    SET_IRQ(14);    SET_IRQ(15);
    
	idt_set_gate(0x80, (u32int)_systemCallHandler, 0x08, ACS_INT); // System Call

   _idt_flush((u32int)&idt_ptr);
}

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags) {
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags /* | 0x60 */;
}