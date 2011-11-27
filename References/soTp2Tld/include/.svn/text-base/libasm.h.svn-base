/*
	FUNCIONES EN ASSEMBLER QUE UTILIZA EL KERNEL
*/


#ifndef _KASM_H
#define _KASM_H

#include "defs.h"
#include "kernel.h"
#include "exceptions.h"

unsigned int    _read_msw();

void            _lidt (IDTR *idtr);

void		_mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		_mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		_Cli(void);		/* Deshabilita interrupciones  */
void		_Sti(void);		/* Habilita interrupciones  */

void		_debug (void);

void 		_int_08_hand(void);
void		_int_80_hand(void);	//rutina de atencion de interrupcion de las system call
void		_int_09_hand(void);	//rutina de atencion de interrupcion del teclado
void		_int_85_hand(void * param); //kill

void 		_kill(void *proc);
void 		_exec (char *name, int (*process) (int argc, char **argv), int inBack);
void            _sleep(int count);

void 		_div_0__hand ( void ) ; // division por cero
void 		_bounds_hand ( void ) ; // bounds error
void 		_opCode_hand ( void ) ; // operation code invalido
void 		_snPres_hand ( void ) ; // segmento no presente
void 		_stackf_hand ( void ) ; // saturacion de segmento de pila
void 		_gralPf_hand ( void ) ; // error de proteccion general
void 		_pFault_hand ( void ); 	// rutina de atencion page fault

void 		_execp01hand ( void );
void 		_execp02hand ( void );
void 		_execp03hand ( void );
void 		_execp04hand ( void );
void 		_execp07hand ( void );
void 		_execp08hand ( void );
void 		_execp09hand ( void );
void 		_execp10hand ( void );
void 		_execp15hand ( void );
void 		_execp16hand ( void );
void 		_execp17hand ( void );
void 		_execp18hand ( void );

void		_read(int fd,char* buffer,int count);  //primitiva read()
void		_write(int fd,char* buffer,int count);  //primitiva write()
void 		_in(char *buffer,int adress,int count); //funcion para leer de perifericos
int 		_writeStackReg(int esp, int ebp);
int 		_readEBP(void);
int 		_readESP(void);

void		_out(int adress,char *buffer,int count); //funcion para escribir en perifericos

void            _switchProcess(void);  //Funcion que cambia el proceso que tiene el cpu "manualmente"

#endif

