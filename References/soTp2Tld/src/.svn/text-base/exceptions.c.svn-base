#include "../include/exceptions.h"

extern PROCESST processes[];
extern nextPID; 
extern currentPID;


void pageFault( registers_t regs )
{
   // A page fault has occurred.
   // The faulting address is stored in the CR2 register.
   int proc_idx;
   int faulting_address;
   asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

   // The error code gives us details of what happened.
   int present   = !(regs.err_code & 0x1); // Page not present
   int rw = regs.err_code & 0x2;           // Write operation?
   int us = regs.err_code & 0x4;           // Processor was in user-mode?
   int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
   int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

   puts("\n");
    for(proc_idx=0; proc_idx<MAXPROCESS && processes[proc_idx].pid!=currentPID;proc_idx++ )
      ; 
    
    if( proc_idx == MAXPROCESS )
      puts("no se encontro proceso!");

   puts( "Proceso Culpable: " );
   puts( processes[proc_idx].name );
   puts( "\n" );

   // Output an error message.
   puts("PANIC ( ");
   if (present) {puts("page not present ");}
   if (rw) {puts("read-only");}
   if (us) {puts("user-mode ");}
   if (reserved) {puts("reserved ");}
   puts(") at ");
   printHex_int( faulting_address );
   puts("\n");
   PANIC(" Page fault!");
}

extern void panic(const char *message, const char *file, int line)
{
    // We encountered a massive problem and have to stop.
    asm volatile("cli"); // Disable interrupts.

    puts("PANIC(");
    puts((char*)message);
    puts(") at ");
    puts((char*)file);
    puts(":");
    printDec_int(line);
    puts("\n");
    // Halt by going into an infinite loop.
    for(;;);
}

extern void panic_assert(const char *file, int line, const char *desc)
{
    // An assertion failed, and we have to panic.
    asm volatile("cli"); // Disable interrupts.

    puts("ASSERTION-FAILED(");
    puts( (char*)desc );
    puts(") at ");
    puts((char*)file);
    puts(":");
    printDec_int(line);
    puts("\n");
    // Halt by going into an infinite loop.
    for(;;);
}


void div0 (void)
{
/*    if(ProcesoExiste(pidActual))
    {*/
        puts("Error division por cero.\n");
//        Kill(pidActual);
/*    }*/
    STOP;
    
} 

void bounds (void)
{
/*    if(ProcesoExiste(pidActual))
    {*/
        puts("Bounds Error.\n");
  //      Kill(pidActual);
/*    }*/
    STOP;
}

void opCode(void)
{
/*    if(ProcesoExiste(pidActual))
    {*/
	stackStatus(0,(char**)0);
        puts("Opcode invalido.\n");
        //Kill(pidActual);
/*    }*/
    STOP;
}

void seg_no_present(void)
{
/*    if(ProcesoExiste(pidActual))
    {
*/
        puts("Segmento no presente.\n");
        //Kill(pidActual);
/*    }*/
    STOP;
}

void stack_fault ( registers_t regs )
{

	
     puts("Saturacion del segmento de pila!\n");


    
    STOP;
}

void gral_protection (void)
{
/*    if(ProcesoExiste(pidActual))
    {*/
	stackStatus();
        puts("General Protection Fault.\n");
        //Kill(pidActual);
/*    }*/
    STOP;
}


void execp01hand ( void )
{
	puts((char*)__FUNCTION__);
	puts( "Debug exception\n" );
	STOP;
}

void execp02hand ( void )
{
	puts((char*)__FUNCTION__);	
	puts( "Non maskable interrupt\n" );
	STOP;
}

void execp03hand ( void )
{
	puts( (char*)__FUNCTION__ );
	puts( "Breakpoint exception\n" );
	STOP;
}

void execp04hand ( void )
{
	puts( (char*)__FUNCTION__ );
	puts( "Into detected overflow\n" );
	STOP;
}


void execp07hand ( void )
{
	puts( (char*)__FUNCTION__ );
	puts( "No coprocessor exception\n" );
	STOP;
}

void execp08hand ( void )
{
	puts((char*) __FUNCTION__ );   
	puts( "Double fault (pushes an error code)\n" );
	//STOP;
}

void execp09hand ( void )
{
	puts( (char*)__FUNCTION__ );   
	puts( "Corprocessor segment overrun\n" );
	STOP;
}

void execp10hand ( void )
{
	puts((char*) __FUNCTION__ );
	puts( "bad TSS (pushes an error code)\n" );
	STOP;
}


void execp15hand ( void )
{
	puts( (char*)__FUNCTION__ );   
	puts( "Unknown interrupt exception\n" );
	STOP;
}

void execp16hand ( void )
{
	puts( (char*)__FUNCTION__ );   
	puts( "Coprocessor fault\n" );
	STOP;
}

void execp17hand ( void )
{
	puts( (char*)__FUNCTION__ );   
	puts( "Alignment check exception\n" );
	STOP;
}

void execp18hand ( void )
{
	puts((char*) __FUNCTION__ );   
	puts( "Machine check exception\n" );
	STOP;
}

