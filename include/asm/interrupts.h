#ifndef ASM_INTERRUPTS_H
#define ASM_INTERRUPTS_H

/* _systemCallHandler
*
* Handler de las system calls
**/
void *_systemCallHandler();

/* _increaseTTCounter
*
* Aumenta el valor del Timer tick counter
**/
void _increaseTTCounter();

/* isr_N
*
* Handlers de la isr N
**/
void isr_0();
void isr_1();
void isr_2();
void isr_3();
void isr_4();
void isr_5();
void isr_6();
void isr_7();
void isr_8();
void isr_9();
void isr_10();
void isr_11();
void isr_12();
void isr_13();
void isr_14();
void isr_15();
void isr_16();
void isr_17();
void isr_18();
void isr_19();
void isr_20();
void isr_21();
void isr_22();
void isr_23();
void isr_24();
void isr_25();
void isr_26();
void isr_27();
void isr_28();
void isr_29();
void isr_30();
void isr_31();

/* irq_N
*
* Handlers de la irq N
**/
void irq_0();
void irq_1();
void irq_2();
void irq_3();
void irq_4();
void irq_5();
void irq_6();
void irq_7();
void irq_8();
void irq_9();
void irq_10();
void irq_11();
void irq_12();
void irq_13();
void irq_14();
void irq_15();
#endif