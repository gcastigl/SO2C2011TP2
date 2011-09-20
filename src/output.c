#include "defs.h"
#include "kc.h"

#define ENTER 0x1C
#define BACKSPACE 0x0E
#define MAX(a,b) ((a)>(b))?(a):(b)

void showGDT(GDTR*);

// OPERACIONES 
void oper();
void exec();
void enter();


int GetChar(void);
extern PROCESS procesos[64];
extern int CurrentPID;
extern int CurrentTTY;
extern TTY tty[8];
char scancodes[200]={};

void loadScanCodes2(void);


//Limpia la memoria de video
void clear_vid()
{
	int i;
	char* vidmem= (char*) 0xb8000;
	for (i=0;i<80*25;i++)
		vidmem[i*2]=' ';
	return;
}

//Limpia una TTY (aunque no este activa)
void k_clear_screen() 
{
	char *vidmem;
	int mitty=GetTTY();
	vidmem=(char*)tty[mitty].terminal;
	unsigned int i = 0;
	
	while(i < (80 * 24 * 2))
	{
		vidmem[i] = ' ';
		i++;
		vidmem[i] = WHITE_TXT;
		i++;
	};
	tty[mitty].movimiento=0;
	message("tty ",24,150);
	putchar((char)mitty+0x30,24,158);
	
}


//Imprime un mensaje en una zona de la pantalla
void message(char * mensaje, int offset, int minioffset)
{
	int longitud;
	int mitty;
	char* pantalla=(char*)0xb8000;
	char *video ;
	mitty=GetTTY();
	video=(char*)tty[mitty].terminal;
	longitud = strlen2(mensaje);
	memcpy3(video + 160*offset+minioffset, mensaje, longitud);
}

//Imprime N blancos en una zona de la pantalla
void blank(int cant, int offset, int minioffset)
{
	int i;
	int mitty;
	char* pantalla=(char*)0xb8000;
	char *video ;
	mitty=GetTTY();
	video=(char*)tty[mitty].terminal;
	for(i=0;i<cant;i++)
		memcpy3(video + 160*offset+minioffset+2*i," ",1);
}


//Pone un caracter en una zona de la pantalla
void putchar(char  c, int offset, int minioffset)
{
	int mitty;	
	char *video ;
	mitty=GetTTY();
	video=(char*)tty[mitty].terminal;
	video [160*offset+minioffset]= c;
}

//Imprime un caracter en donde esta el cursor
void writechar(char c)
{
	int mitty;	
	char *video ;
	mitty=GetTTY();
	video=(char*)tty[mitty].terminal;
	video [tty[mitty].movimiento]= c;
	tty[mitty].movimiento+=2;
}


//Imprime un mensaje donde esta el cursor
void mess(char* s)
{
	int mitty;	
	int len;
	char *video ;
	mitty=GetTTY();
	len=strlen2(s);
	video=(char*)tty[mitty].terminal;
	memcpy3(&video [tty[mitty].movimiento], s,len);
	tty[mitty].movimiento+=len*2;
}



//Imprime un mensaje en la consola de errores
void puterr(char* s)
{
	int mitty;	
	int len;
	char *video ;
	mitty=GetTTY();
	len=strlen2(s);
	video=(char*)tty[mitty].terminal;
	blank(70,24,0);
	message(s,24,0);
}

//Imprime un mensaje y un enter en donde esta el cursor
void messl(char* s)
{
	int mitty;	
	int len;
	char *video ;
	mitty=GetTTY();
	len=strlen2(s);
	video=(char*)tty[mitty].terminal;
	memcpy3(&video [tty[mitty].movimiento], s,len);
	tty[mitty].movimiento+=len*2;
	tty[mitty].movimiento=tty[mitty].movimiento-tty[mitty].movimiento%160+160;
	if (tty[mitty].movimiento / 160 >= 24)
	{
		Scroll();
	}
}


//Scrollea la pantalla
void Scroll(void)
{
	int i;
	int mitty;	
	char *video ;
	mitty=GetTTY();
	video=(char*)tty[mitty].terminal;
	for(i = 160; i < (160 * 24); i++)
	{
		video[i - 160] = video[i];
	}
	for (i = (160 * 23); i < ((160 * 23) + 160); i += 2)
	{
		video[i] = ' ';
	}
	tty[mitty].movimiento = 160 * 23;
}




//Levanta del buffer de su TTY un caracter (procesado)
int GetChar()
{
	int key;
	int tags=0;
	char* video= (char*)0xB8000;
	BUFFERTYPE* mibuffer;

	mibuffer=GetBuffer();
	if((mibuffer->tail)==(mibuffer->head))
	{
		return -1;
	}
	else
	{
		key=mibuffer->buffer[mibuffer->head]&0xFF;
		tags=mibuffer->buffer[mibuffer->head]&0xFF00;
		tags=tags>>8;
		mibuffer->head=(mibuffer->head+1)%10;
	}
	if(tags==0)
		return scancodes[key];
	else if(tags&SHIFT_PRESSED)
		return scancodes[key+100];
	return -1;

}



//Levanta del buffer de su TTY un scancode
int GetKey()
{
	int key;
	char* video= (char*)0xB8000;
	BUFFERTYPE* mibuffer;

	mibuffer=GetBuffer();
	if((mibuffer->tail)==(mibuffer->head))
	{
		return -1;
	}
	else
	{
		key=mibuffer->buffer[mibuffer->head]&0xFF;
		mibuffer->head=(mibuffer->head+1)%10;
	}
	
	return key;
}





//Carga los scancodes
void loadScanCodes2(void)
{
	scancodes[30]='a';
	scancodes[48]='b';
	scancodes[46]='c';
	scancodes[32]='d';
	scancodes[18]='e';
	scancodes[33]='f';
	scancodes[34]='g';
	scancodes[35]='h';
	scancodes[23]='i';
	scancodes[36]='j';
	scancodes[37]='k';
	scancodes[38]='l';
	scancodes[50]='m';
	scancodes[49]='n';
	scancodes[24]='o';
	scancodes[25]='p';
	scancodes[16]='q';
	scancodes[19]='r';
	scancodes[31]='s';
	scancodes[20]='t';
	scancodes[22]='u';
	scancodes[47]='v';
	scancodes[17]='w';
	scancodes[45]='x';
	scancodes[21]='y';
	scancodes[44]='z';
	scancodes[57]=' ';
	scancodes[41]='`';
	scancodes[2]='1';
	scancodes[3]='2';
	scancodes[4]='3';
	scancodes[5]='4';
	scancodes[6]='5';
	scancodes[7]='6';
	scancodes[8]='7';
	scancodes[9]='8';
	scancodes[10]='9';
	scancodes[11]='0';
	scancodes[12]='-';
	scancodes[13]='=';
	scancodes[14]='í';//backspace
	//scancodes[68]='ñ';//F10
	scancodes[26]='[';
	scancodes[27]=']';
	scancodes[28]='é'; //ENTER
	scancodes[43]='\\';
	scancodes[39]=';';
	scancodes[40]='\'';
	scancodes[51]=',';
	scancodes[52]='.';
	scancodes[53]='/';

	scancodes[130]='A';
	scancodes[148]='B';
	scancodes[146]='C';
	scancodes[132]='D';
	scancodes[118]='E';
	scancodes[133]='F';
	scancodes[134]='G';
	scancodes[135]='H';
	scancodes[123]='I';
	scancodes[136]='J';
	scancodes[137]='K';
	scancodes[138]='L';
	scancodes[150]='M';
	scancodes[149]='N';
	scancodes[124]='O';
	scancodes[125]='P';
	scancodes[116]='Q';
	scancodes[119]='R';
	scancodes[131]='S';
	scancodes[120]='T';
	scancodes[122]='U';
	scancodes[147]='V';
	scancodes[117]='W';
	scancodes[145]='X';
	scancodes[121]='Y';
	scancodes[144]='Z';
	scancodes[157]=' ';
	scancodes[141]='~';
	scancodes[102]='!';
	scancodes[103]='@';
	scancodes[104]='#';
	scancodes[105]='$';
	scancodes[106]='%';
	scancodes[107]='^';
	scancodes[108]='&';
	scancodes[109]='*';
	scancodes[110]='(';
	scancodes[111]=')';
	scancodes[112]='_';
	scancodes[113]='+';
	scancodes[126]='{';
	scancodes[127]='}';
	scancodes[143]='|';
	scancodes[139]=':';
	scancodes[140]='"';
	scancodes[151]='<';
	scancodes[152]='>';
	scancodes[153]='?';
}





