#include "../include/defs.h"
#include "../include/kc.h"
#define MAX(a,b) ((a)>(b))?(a):(b)
extern PROCESS procesos[64];
extern int CurrentPID;
extern int CurrentTTY;
extern TTY tty[8];

void InterpretarBuffer(char*);

//Proceso Shell
int Shell(int argc, char* argv[])
{
	int a=0, i, conty;
	int* cont;
	BUFFERTYPE* mibuffer;
	char* video= (char*)0xB8000;
	k_clear_screen();
	_Sti();
	loadScanCodes2();
	char este[80]; 
	cont=(int*)Calloc(sizeof(int));
	for (i=0;i<80;i++)
	{
		este[i] = 0;
	}
	puterr("Para ayuda escriba help");
	mess("==>");
	while(1)
	{
		_Cli();
			a=GetChar();
			if(a==-1)
			{
				PROCESS* proc;
				proc=GetProcessByPID(CurrentPID);
				if (proc->blocked==0)
					proc->blocked=1;

			}
			else
			{
				switch (a)
				{
				case 'í'://BACKSPACE
					if (conty>0)
					{
						tty[CurrentTTY].movimiento -= 2;	
						tty[CurrentTTY].terminal[tty[CurrentTTY].movimiento] = ' ';
						este[--conty] = 0;
					}
					break;
				case 'é'://ENTER:
					este[conty]='\0';
					messl("");
					InterpretarBuffer(este);
					mess("==>");
					conty = 0;
					for(i=0; i<80; i++)
						este[i] = '\0';
					break;
				default:
					if (conty<50)
					{
						este[conty++] = (char) a;
						writechar(a);
					}
				}
			}
		_Sti();
	}
}

//Interpretador de argumentos
void separador(char* buffer, int* argc, char** argv)
{
	int contador = 0, i = 0, aux = 0;
	char prueba[111];
	while(buffer[contador] != '\0')
	{
		aux = lenspace(buffer + contador);
		argv[i] = (char*)Malloc(aux + 1);
		memcpy2(argv[i], buffer + contador, aux);
		contador += aux;
		argv[i][aux] = '\0'; 
		contador++;
		i++;
	}
	*argc = i;	
}


//Interprete de buffer
void InterpretarBuffer(char* buffer)
{
	int i;
	int size;
	int argc;
	char** argv;
	char *video = (char*) 0xB8000;
	
	size = lenspace(buffer);
	if (buffer[size] == ' ')
		separador(buffer, &argc, argv);

	if(memcmp2((char*)buffer,"payaso", MAX(6, size)))
	{
		CreateProcessAt("Payaso",Payaso, CurrentTTY, argc, argv, 0x400, 2, 1);
		_Sti();
		_int8();
		k_clear_screen();
		_Cli();
	}
	else if(memcmp2((char*)buffer,"payaso.", MAX(6, size)))
	{
		CreateProcessAt("Payaso",Payaso, CurrentTTY, argc, argv, 0x400, 2, 0);
	}
	else if(memcmp2((char*)buffer, "nice", MAX(4, size)))
	{
		nice(buffer);
	}
	else if(memcmp2((char*)buffer, "kill", MAX(4, size)))
	{
		kill(buffer);
	}
	else if(memcmp2((char*)buffer, "top", MAX(3, size)))
	{
		CreateProcessAt("Top",Tope, CurrentTTY, argc, argv, 0x400, 2, 1);
		_int8();
		k_clear_screen();
	}
	else if(memcmp2((char*)buffer, "top.", MAX(3, size)))
	{
		CreateProcessAt("Top",Tope, CurrentTTY, argc, argv, 0x400, 2, 0);
	}
	else if(memcmp2((char*)buffer, "cls", MAX(3, size)))
	{
		k_clear_screen();
		tty[CurrentTTY].movimiento=0;
	}
	else if(memcmp2((char*)buffer, "dancing", MAX(7, size)))
	{
		CreateProcessAt("dancing",DancingMessage, CurrentTTY, argc, argv, 0x400, 2, 1);
	}
	else if(memcmp2((char*)buffer, "dancing.", MAX(8, size)))
	{
		CreateProcessAt("dancing",DancingMessage, CurrentTTY, argc, argv, 0x400, 2, 0);
	}
	else if(memcmp2((char*)buffer, "help", MAX(4, size)))
	{
		messl("Comandos: ");
		messl("cls: Limpia la pantalla");
		messl("kill <pid>: Mata al proceso <pid>");
		messl("nice <pid> <0-4>: Setea la prioridad del proceso <pid> en <0-4>");
		messl("Procesos (llamarlos con . al final para que funcionen en modo back)");
		messl("payaso: Una barra de payaso! :)");
		messl("top [<refresh time>]: Informacion acerca del consumo de CPU de los procesos");
		messl("dancing [param]: Muestra las palabras dancing y [param] bailando");

		
	}
	
	else
		puterr("Comando no reconocido, escriba help para una lista de comandos.");
	
	
}














