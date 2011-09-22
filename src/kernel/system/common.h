#ifndef COMMON_H
#define COMMON_H

// Exact-width integer types
typedef   signed char  int8_t;
typedef unsigned char  uint8_t;
typedef   signed short int16_t;
typedef unsigned short uint16_t;
typedef   signed int   int32_t;
typedef unsigned int   uint32_t;

#define NULL ((void*)0)

// PIC
#define PORT_PIC1 0x20
#define PORT_PIC2 0xA0
#define SIGNAL_EOI 0x20

extern void outw(uint16_t port, uint16_t value);
extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);
extern uint16_t inw(uint16_t port);
extern uint32_t getRDTSC();

#endif // COMMON_H
