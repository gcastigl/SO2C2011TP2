#ifndef _kc_
#define _kc_

#include <defs.h>

/* Muestra la imagen de inicio */
void showSplashScreen();

/* Tiempo de espera */
void wait(int time);

/* Inicializa la entrada del IDT */
void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
	byte cero);

void setup_GDT_entry (gdt_ptr_t* item, dword base, dword limit, byte access, byte attribs);

#endif
