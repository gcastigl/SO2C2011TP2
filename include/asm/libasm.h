#ifndef LIBASM_H
#define LIBASM_H
#include <defs.h>

unsigned int _read_msw();

void _outb(ushort port, ushort data);
unsigned char _inb(ushort port);

void _port_out(ushort port, ushort data);
unsigned char _port_in(ushort port);
void _portw_out(ushort port, ushort data);
unsigned int _portw_in(ushort port);

void _mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void _mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void _cli(void);        /* Deshabilita interrupciones  */
void _sti(void);	 /* Habilita interrupciones  */

void _reset();
int _cpuIdTest();
int _rdtscTest();
int _rdmsrTest();

void _int_08_hand();	/* Timer tick */
void _int_09_hand();	/* Keyboard */
void _int_80_hand();
void _invop_hand(void);
void _snp_hand(void);
void _bounds_hand(void);
void _ssf_hand(void);
void _div0_hand(void);
void _gpf_hand(void);
void _pageFault_hand(void);

void _SysCall();

void _initTTCounter();
int _getTTCounter();
int _tscGetCpuSpeed();
int _msrGetCpuSpeed();
void _debug (void);

#endif
