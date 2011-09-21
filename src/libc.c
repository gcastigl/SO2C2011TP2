#include "../include/kc.h"

/***************************************************************
*setup_IDT_entry
* Inicializa un descriptor de la IDT
*
*Recibe: Puntero a elemento de la IDT
*	 Selector a cargar en el descriptor de interrupcion
*	 Puntero a rutina de atencion de interrupcion	
*	 Derechos de acceso del segmento
*	 Cero
****************************************************************/

void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
	byte cero) {
	item->selector = selector;
	item->offset_l = offset & 0xFFFF;
	item->offset_h = offset >> 16;
	item->access = access;
	item->cero = cero;
}

void setup_GDT_entry (DESCR_SEG* item, dword base, dword limit, byte access, byte attribs) {
	item->base_l=base & 0xffff;
	item->base_m=(base>>16)&0xff;
	item->base_h=(base>>24);
	item->limit=limit & 0xffff;
	item->attribs= attribs | ((limit>>16) &0x0f);
	item->access=access;
}
