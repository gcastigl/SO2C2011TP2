#ifndef __EXCEPTIONS_H
#define __EXCEPTIONS_H
#include "process.h"

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))
#define STOP while(1);

typedef struct registers
{
   int ds;                  // Data segment selector
   int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   int int_no, err_code;    // Interrupt number and error code (if applicable)
   int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t; 

extern void panic(const char *message, const char *file, int line);
extern void panic_assert(const char *file, int line, const char *desc);

void div0 (void);
void bounds (void);
void opCode(void);
void seg_no_present(void);
void stack_fault( registers_t reg );
void gral_protection (void);
void pageFault( registers_t reg );

void execp01hand ( void );
void execp02hand ( void );
void execp03hand ( void );
void execp04hand ( void );
void execp07hand ( void );
void execp08hand ( void );
void execp09hand ( void );
void execp10hand ( void );
void execp15hand ( void );
void execp16hand ( void );
void execp16hand ( void );
void execp18hand ( void );

#endif
