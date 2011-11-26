#include "../include/kernel.h"

DESCR_INT idt[0x95];	/* IDT de 80h entradas*/  //con 80 no funciona!!!
IDTR idtr;				/* IDTR */

//PROCESST init;
PROCESST processes[MAXPROCESS]={0};
int nextPID=1; 
int currentPID=0;

static char bufferExce[2048];

char * stackTop=bufferExce+sizeof(bufferExce);

// Provienen de link.ld
extern int end;
extern int data;
extern int code;
extern int text;


void initIDT(void);

/**********************************************
kmain() 
*************************************************/
kmain() 
{
  PROCESST *proc;
	initIDT();

	_Cli();
	
	/* Habilito interrupciones de teclado y timertick*/
	_mascaraPIC1(0xFC);
	_mascaraPIC2(0xFF);
        
	setupConsoles();

	initPaging();

	initScheduler(); //Inicializa la tarea init  
	 	
	createProcess("init", initProcess, 0, 0, (char **)0, 0x4000,0,BACKGROUND,NOTBLOCKED); //Crea el proceso Init, su pid va a ser el 1

	createProcess("shell1",shell,1,0,(char **)0, 0x4000,1,BACKGROUND,NOTBLOCKED); 
	createProcess("shell2",shell,2,0,(char **)0, 0x4000,1,BACKGROUND,BLOCKED);
	createProcess("shell3",shell,3,0,(char **)0, 0x4000,1,BACKGROUND,BLOCKED);
	createProcess("shell4",shell,4,0,(char **)0, 0x4000,1,BACKGROUND,BLOCKED);
	createProcess("shell5",shell,5,0,(char **)0, 0x4000,1,BACKGROUND,BLOCKED);

	createProcess("clock",clock,1,0,(char **)0, 0x4000,2,BACKGROUND,NOTBLOCKED); //Crea un proceso para la terminal principal	

	_Sti();

}

void
initIDT(void)
{
/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */
	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0); // timer tick
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0); // Read

	/** Excepciones  **/
	setup_IDT_entry (&idt[0x00], 0x08, (dword)&_div_0__hand, ACS_INT, 0); // division por cero
	setup_IDT_entry (&idt[0x01], 0x08, (dword)&_execp01hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x02], 0x08, (dword)&_execp02hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x03], 0x08, (dword)&_execp03hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x04], 0x08, (dword)&_execp04hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x05], 0x08, (dword)&_bounds_hand, ACS_INT, 0); // out of bounds
	setup_IDT_entry (&idt[0x06], 0x08, (dword)&_opCode_hand, ACS_INT, 0); // operation code invalido
	setup_IDT_entry (&idt[0x07], 0x08, (dword)&_execp07hand, ACS_INT, 0); //
//	setup_IDT_entry (&idt[0x08], 0x08, (dword)&_execp08hand, ACS_INT, 0); // y el timer tick????
//	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_execp09hand, ACS_INT, 0); // Se pisa con la del teclado???
	setup_IDT_entry (&idt[0x0A], 0x08, (dword)&_execp10hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x0B], 0x08, (dword)&_snPres_hand, ACS_INT, 0); // segmento no presente
	setup_IDT_entry (&idt[0x0C], 0x08, (dword)&_stackf_hand, ACS_INT, 0); // seturacion de segmento de pila
	setup_IDT_entry (&idt[0x0D], 0x08, (dword)&_gralPf_hand, ACS_INT, 0); // error de proteccion general*/
	setup_IDT_entry (&idt[0x0E], 0x08, (dword)&_pFault_hand, ACS_INT, 0); // segmentation Fault
	setup_IDT_entry (&idt[0x0F], 0x08, (dword)&_execp15hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x10], 0x08, (dword)&_execp16hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x11], 0x08, (dword)&_execp17hand, ACS_INT, 0); // 
	setup_IDT_entry (&idt[0x12], 0x08, (dword)&_execp18hand, ACS_INT, 0); // 
	
	/** iterrupciones de software **/
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&_int_80_hand, ACS_INT, 0); // Write
	setup_IDT_entry (&idt[0x85], 0x08, (dword)&_int_85_hand, ACS_INT, 0); // Kill
/* Carga de IDTR    */

	idtr.base = 0;  
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;
	
	_lidt (&idtr);	

}


 


