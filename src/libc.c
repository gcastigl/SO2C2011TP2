#include "kc.h"
#include "defs.h"

extern int movimiento;
extern TTY tty[8];
extern char* backupTerm;
int countmem(void);

//Cuenta la memoria disponible en la PC
int countmem()
{
	int cont = 2;
	char* memoria = (char *) 0x200000;	

	while(1)
	{
		memoria[0] = '&';
		if (memoria[0] != '&')
			break;
		else
		{
			cont++;
			memoria += 0x100000; /* Voy contando de a 1MB */
		}
	}
	return cont;
}

//String to Integer
int atoi2(char* texto)
{
	int a=0;
	int i=0;
	while (texto[i]>=0x30 && texto[i]<=0x39)
	{
		a*=10;
		a+=(texto[i]-0x30);
		i++;			
	}
	return a;
}


//String en hexa to int
int atoh2(char* texto)
{
	
	int a=0;
	int i=0;
	while ((texto[i]>=0x30 && texto[i]<=0x39) || (texto[i]>='A' && texto[i]<='F'))
	{
		a*=16;
		if (texto[i]<=0x39)
			a+=(texto[i]-0x30);
		else
			a+=texto[i]-55;
		i++;			
	}
	return a;
}

//Integer to String
int itoa2(unsigned int a, char* rta)
{
	char temp;
	int contador = 0;
	int i;
	
	do
	{
		rta[contador] = (a % 10) + 0x30;
		a /= 10;
		contador++;
	}
	while (a!=0);
	for(i = 0; i < (contador / 2); i++)
	{
		temp = rta[i];
		rta[i] = rta[contador - i - 1];
		rta[contador - i - 1] = temp;
	}
	rta[contador]=0;
	return contador;	
}



//Compara memoria
int memcmp2(char* a, const char* b, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		if (a[i] != b[i])
		{
			return 0;
		}
	}
	return 1;
}


//Memcopy
void memcpy2(char* a, char* b, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		a[i] = b[i];
	}
	return;
}


//Memcopy pensado para copiar de un buffer a video
void memcpy3(char* a, char* b, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		a[2 * i] = b[i];
	}
	return;
}



//Longitud de un string
int strlen2(char* a)
{
	int i;
	for(i = 0; i < 80; i++)
	{
		if(a[i] == '\0')
		{
			return i;
		}
	}
	return -1;
}


//Longitud de un string hasta un espacio
int lenspace(char* a)
{
	int i;
	for(i = 0; i < 80; i++)
	{
		if(a[i] == ' ' || a[i] == '\0')
		{
			return i;
		}
	}
	return -1;
}


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

void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access, byte cero) {
  item->selector = selector;
  item->offset_l = offset & 0xFFFF;
  item->offset_h = offset >> 16;
  item->access = access;
  item->cero = cero;
}

void setup_GDT_entry (DESCR_SEG* item, dword base, dword limit, byte access, byte attribs)
{
	item->base_l=base & 0xffff;
	item->base_m=(base>>16)&0xff;
	item->base_h=(base>>24);
	item->limit=limit & 0xffff;
	item->attribs= attribs | ((limit>>16) &0x0f);
	item->access=access;

}
