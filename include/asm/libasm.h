#ifndef LIBASM_H
#define LIBASM_H
#include <defs.h>
#include <process/process.h>

/* _read_msw
*
* Función que lee la machine status word
**/
unsigned int _read_msw();

void _port_out(ushort port, ushort data);
unsigned char _port_in(ushort port);
void _portw_out(ushort port, ushort data);
unsigned int _portw_in(ushort port);

void _mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void _mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void copy_page_physical(int,int);
/* loadStackFrame
*
* Recibe como parametros:
* - función que ejecutar
* - Inicio del stack
* - Cantidad de argumentos
* - Puntero a los argumentos
* - Función de retorno
*
* Función que inicializa el stack para un proceso
**/
int loadStackFrame(int (*func)(int argc, char** argv),int stackStart, int argc, char** argv, void (*cleaner)(void));

/* switchProcess
*
* Cambia el proceso actual
**/
void switchProcess();
void _cli(void);        /* Deshabilita interrupciones  */
void _sti(void);	 /* Habilita interrupciones  */


void _reset(); /* Reinicia la computadora */
int _cpuIdTest(); /* chequea cpuId */
int _rdtscTest(); /* chequea rdtsc */
int _rdmsrTest(); /* chequea rdmsr */

/* _SysCall
*
* Funcion handler de las system calls
**/
void *_SysCall();

/* _initTTCounter
*
* Inicializa el timer tick counter
**/
void _initTTCounter();

/* _initTTCounter
*
* Devuelve el Timer Tick counter
**/
int _getTTCounter();

/* _tscGetCpuSpeed
*
* Obtiene la velocidad de la cpu usando el tsc
**/
int _tscGetCpuSpeed();

/* _msrGetCpuSpeed
*
* Obtiene la velocidad de la cpu usando el msr
**/
int _msrGetCpuSpeed();
void _debug (void);

int read_cs();
int read_ss();
int read_ds();
int read_esp();
int read_ebp();

#define _EIP    read_eip()
#define _CS     read_cs()
#define _SS     read_ss()
#define _DS     read_ds()
#define _ESP    read_esp()
#define _EBP    read_ebp()

#endif
