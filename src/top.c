#include "defs.h"
#include "kc.h"
extern int last100[100];

//Proceso Top
int Tope(int argc, char* argv[])
{
	char* video= (char*)0xb8000;
	int proce[65];
	char Mensaje [10];
	int salto;
	int i;
	char a;
	k_clear_screen();
	message("Top V1.0: Procesos y porcentaje de CPU utilizada",0,10);
	message("PID          CPU%         TTY          NAME",2,0);
	_Sti();
	while(a!=68)
	{
		a=GetKey();
		salto=3;
		for(i=0;i<65;i++)
		{
			proce[i]=0;
			
		}
		for(i=0;i<100;i++)
		{	
			proce[last100[i]]++;
		}
		_Cli();
			
			
			for(i=0;i<65;i++)
			{
				PROCESS* p=GetProcessByPID(i);
				if(p->free==0)
				{
					itoa2(i,Mensaje);
					message(Mensaje,salto,0);
					itoa2(proce[i],Mensaje);
					message(Mensaje,salto,28);
					itoa2(p->tty,Mensaje);
					message(Mensaje,salto,56);
					message(p->name,salto,78);
					salto=salto+1;

					
				}
				message("                                                                         ",salto,0);
			}
			if (argc>1)
				i=atoi2(argv[1]);
			else
				i=0;
		_Sti();
		

		Sleep(i);
	}
	return 0;
}
