/*********************************************
kasm.h

************************************************/
#ifndef __KASM_H_
#define __KASM_H_

#include "defs.h"


unsigned int read_msw();

void _lidt(IDTR *idtr);
void _lgdt(GDTR *gdtr);

void mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void _Cli(void);		/* Deshabilita interrupciones */
void _Sti(void);		/* Habilita interrupciones */

void _int_08_hand(void);		/* Timer tick */
void _int_09_hand(void);		/* Keyboard */
void _int_1_hand(void);
void _invop_hand(void);
void _snp_hand(void);
void _bounds_hand(void);
void _ssf_hand(void);
void _div0_hand(void);
void _gpf_hand(void);
void _hacersnp(void);
void _hacerbounds(void);
void _hacergpf(void);
void _hacerssf(void);
void _hacerinvop(void);
void _getgdt(void);
void _debugBuenaOnda (void);
void _movercursor(void);
void _beep(void);
void _seteards(void);
void _setearcs(void);
void _setearss(void);
void _ejecutar(void);


#endif
