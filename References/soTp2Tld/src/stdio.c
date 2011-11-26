#include "../include/stdio.h"

void
putc( char c )
{
	if (c!='\0')
		sCreenHandler( &c,1,MAIN_SCREEN,0 );
}

void
gputc( char c, int screen_sector )
{
	if (c!='\0')
		sCreenHandler( &c,1,screen_sector,0 );
}


//imprime un stirng
void 
puts( char* s )
{
	if (*s!='\0')	//la validacion es necesaria para no hacer un write con count=0
		sCreenHandler(s,strlen(s),MAIN_SCREEN,0);
}

void 
puts_title( char* s )
{
	char title[ TITLE_WIDTH +1];
	
	memset( title, ' ', TITLE_WIDTH );
	strcpy( &title[1], s );
	title[ strlen(s)+1 ]=' ';
	title[ TITLE_WIDTH ]='\0';
	
	putc('\n');
	setColor( TITLE_TXT );
	puts(title);
	setColor( WHITE_TXT );
	
	puts("\n\n");
}


void 
gputs( int screen_sector, char* s )
{
	if (*s!='\0')	//la validacion es necesaria para no hacer un write con count=0
		sCreenHandler(s,strlen(s),screen_sector,0);
}




void 
printHex_rec( int number, int bytes )
{
  if( bytes > 0 )
  {
    printHex_rec( number>>8, --bytes );
    printHex( number );
  }
}

void
printHex_int( int number )
{
  puts("0x");
  printHex_rec( number, sizeof(int) );
}

//imprime un byte en hexadecimal
void
printHex(unsigned char dByte)
{
	unsigned char aux;
	aux=dByte>>4;
	if (aux<=9)
		putc(aux+'0');
	else
		putc(aux-10+'A');
		
	aux=dByte & 0x0f;
	if (aux<=9)
		putc(aux+'0');
	else
		putc(aux-10+'A');

}

//imprime un byte en hexadecimal
void
gprintHex(unsigned char dByte,int screen_sector)
{
	unsigned char aux;
	aux=dByte>>4;
	if (aux<=9)
		gputc(aux+'0',screen_sector);
	else
		gputc(aux-10+'A',screen_sector);
		
	aux=dByte & 0x0f;
	if (aux<=9)
		gputc(aux+'0',screen_sector);
	else
		gputc(aux-10+'A',screen_sector);

}



void 
printDec_rec( int number, int bytes )
{
  if( bytes > 0 )
  {
    printDec_rec( number>>8, --bytes );
    printDec( number );
  }
}

void
printDec_int( int number )
{
  printDec_rec( number, sizeof(int) );
}

//imprime un byte en decimal
void
printDec(unsigned char dByte)
{

	putc(dByte/10+'0');
	putc(dByte%10+'0');	
}

//retorna la longitud de un string
int strlen(char *s)
{
	int i;
	for( i=0; s[i]!='\0'; i++ );
	return i;
}

int
atoi(char * str)
{	
	int num=0;
	int len,i;
	int pot=1,aux;	
	
	len=strlen(str);
	
	for(i=len-1 ;i >= 0 ;i--)				
	{	
		aux=(str[i]-'0');
		num+=(aux*pot);
		pot*=10;
	}

	return num;
}

//funci�n que espera a que el buffer se llene o haya un enter, 
//y devuelve el string correspondiente
void
getline(char *comand)
{
	console_t *console = getProcessConsole();

	while (!console->kfull);

	console->kfull=0;
	console->kcur=0;
	strcpy(comand,console->kbuffer);
	
}

//Funcion que devuelve true o false dependindo de si se escribió la letra pasada por parametro. Consume todo lo que se introduzca en el buffer
int
isCharIn(char key)
{
	console_t *console = getProcessConsole();

	console->kfull=0;
	console->kcur=0;
	return key==*(console->kbuffer);
	
}


//copia el contenido del string arg2 en arg1
void
strcpy(char *arg1,char *arg2)
{
	
	while (*arg2!='\0')
	{
		*arg1++=*arg2++;
	}
	*arg1='\0';

}


//retorna si dos strings son iguales
int 
strcmp (char *arg1,char*arg2)
{
	int res;

	while ( (res=*arg1-*arg2)==0 && *arg1!='\0') //sigue en el ciclo mientras sean diferentes entre si, y distintos a '\0'
	{
		arg1++;
		arg2++;
	}
	return res;

}

// inicializa un sector de memoria de tamano to bytes con el with
void memset( void * buffer, char with, int to )
{
  int i;
  for( i=0; i< to; i++ )
    *((char*)buffer+i)=with;
}

//lista los dispositivos pci
void
lspci( void )
{
	int bus;
	int dev;
	int fun; 

	int pciLine;

	char *venStr;
	char *devStr;

	int flagAnt=0;
	int devAnt=0;

	puts_title( "PCI Devices");

	for( bus=0; bus< MAX_BUS; bus++ ) // Busses
		for( dev=0; dev< MAX_DEV ; dev++ ) // Devices
			for( fun=0; fun< MAX_FUN ; fun++ ) // Functions
			{
				venStr=NULL;
				devStr=NULL;

				if(  (pciLine=getPciData(bus,dev,fun,0))!=-1 )
				{
					if (flagAnt!=pciLine || dev!=devAnt)
					{										
 						getPciProduct(pciLine,&venStr,&devStr);
						{
							printHex(bus);
							putc(':');
							printHex(dev);
							putc('.');				
							printHex(fun);
							
							puts(" - ");
	
							printHex(pciLine>>8);
							printHex(pciLine);
							putc('-');
							printHex(pciLine>>24);
							printHex(pciLine>>16);
							
							puts(":  ");	
							puts(venStr);
							puts(":  ");
							puts(devStr);
							putc('\n');
						}
					}
					flagAnt=pciLine;
					devAnt=dev;
				}
			}
		
	putc('\n');
}


//funci�n que retorna el contenido del registro indicado por reg, del
//dispositivo pci indicado por bus,dev,y fun
int
getPciData (int bus, int dev,int fun,int reg)
{
	int data;

	data=0x80000000|(bus<<16)|(dev<<11)|(fun<<8)|reg;

	_write(PCI_I,(char*)&data,4);
	_read(PCI_D,(char*)&data,4);
	
	return data;
}

//funci�n que dado el registro de pci donde se encuentra el vendor id y device id
//retorna los strings que corresponden
void
getPciProduct(unsigned int pciLine,char **venStr,char **devStr)
{
	getVenDevStr (pciLine & 0xFFFF,pciLine>>16,venStr,devStr);
}


