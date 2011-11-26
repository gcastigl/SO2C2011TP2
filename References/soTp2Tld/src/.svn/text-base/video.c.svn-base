
#include "../include/video.h"

int hlines=3; // header lines
int shlines=1; // subheader ines
int flines=1; // foot lines

//extern console_t consoles[];

extern console_t * actualConsole;

void newLine(console_t *console);
void upScreen(console_t *console);

char clock_buffer[CLOCKLEN]={0};
int clock_cur=0;

void sCreenHandler( char *buffer,int count, int screen_sector,int flag)
{
	int i=(hlines+shlines)*COL*2;
	char portData;
	console_t * console;

	if (flag==0)
		console=getProcessConsole();
	else
	  console = actualConsole;

	switch (screen_sector)
	{
	    case HEAD_SCREEN:
		for( i=0; i< COL;++i )
		{
		  console->vbuffer[console->headcur++]=' ';
		  console->vbuffer[console->headcur++]=HEAD_COLOR;
		}
		for( i=0; i< (hlines-2)*COL;++i )
		{
		  console->vbuffer[console->headcur++]=(i < count && i< strlen(" Sistemas Operativos"))?buffer[i]:' ';
		  console->vbuffer[console->headcur++]=HEAD_COLOR;
		}
		for( i=strlen(" Sistemas Operativos"); i< (hlines-1)*COL;++i )
		{
		  console->vbuffer[console->headcur++]=(i < count )?buffer[i]:' ';
		  console->vbuffer[console->headcur++]=HEAD_COLOR_2;
		}
		break;   

	    case SUB_HEAD_SCREEN:

		for( i=0; i< shlines*COL;++i )
		{
		    console->vbuffer[console->sheadcur++]=(i < count)?buffer[i]:' ';
		    console->vbuffer[console->sheadcur++]=SUB_HEAD_COLOR;
		}
		break;

	    case FOOT_SCREEN:
		for( i=2*COL*(ROW-flines); i< ROW*COL*2;++i )
		{
		  console->vbuffer[console->footcur++]=((i-2*COL*(ROW-flines)) < count)?buffer[i-2*COL*(ROW-flines)]:' ';
		  console->vbuffer[console->footcur++]=FOOT_COLOR;
		}
		break;

	    case FOOT_TTY_F:

		console->vbuffer[console->ttycur++]=buffer[0];
		console->vbuffer[console->ttycur++]=TTY_ON_TXT;
	  
		console->vbuffer[console->ttycur++]='|';
		console->vbuffer[console->ttycur++]=FOOT_COLOR;

		break;

	    case FOOT_TTY_B :
		console->vbuffer[console->ttycur++]=buffer[0];
		console->vbuffer[console->ttycur++]=TTY_OFF_TXT;
	  
		console->vbuffer[console->ttycur++]='|';
		console->vbuffer[console->ttycur++]=FOOT_COLOR;
			
		break;

	    case FOOT_TIME:

		clock_buffer[clock_cur++]=buffer[0];
		clock_buffer[clock_cur++]=FOOT_COLOR;

		if(clock_cur==CLOCKLEN)
		  clock_cur=0;

		_write(FOOT_TIME,clock_buffer,CLOCKLEN);

		break;

	    case MAIN_SCREEN:

	      if (count==0) //clearScreen
	      {
		  while (i<2*COL*(ROW-flines))
		  {
		    console->vbuffer[i++]=' ';
		    console->vbuffer[i++]=console->currentColor; /* color */
		  }
		  console->cursor=(hlines+shlines)*COL*2;
	      }
	      else
		  for (i=0;i<count;i++)
		  {
		    if (console->cursor==((ROW-flines)*COL*2)) //cuando llega al final de pantalla, vuelve al inicio 	
		      upScreen(console);
		    
		    switch (buffer[i])
		    {
		      case ENTER:
			newLine(console);
			break;
		      case BACK_SPACE:
			if(console->kcur>0)
			  console->vbuffer[console->cursor-=2]=' ';
			break;
		      default:
			console->vbuffer[console->cursor++]=buffer[i];
			console->vbuffer[console->cursor++]=console->currentColor;
			break;	
		    }
		  }
	 }
		  
	 if (flag==1 || getProcessConsole()==actualConsole)
	      flushBuffer();
}

void
flushBuffer(void)
{
     char portData;
     console_t *console=actualConsole;

    _write(STDOUT,console->vbuffer,(ROW*COL*2-CLOCKLEN));  //escribe en el puerto de la pantalla el contenido de videoAux

    /*con estas instrucciones se maneja el cursor
		      bibliografia: http://joelgompert.com/OS/lesson7.htm*/

    portData=14;
    _write(K_CUR_1,&portData,1);   // palabra de control para escribir en el registro 14 del puerto

    portData=((console->cursor/2)>>8) & 0xFF;
    _write(K_CUR_2,&portData,1); 		//se manda los 4 bits mas significativos del cursor

    portData=15;
    _write(K_CUR_1, &portData,1);           // palabra de control para escribir en el registro 15 del puerto

    portData=(console->cursor/2) & 0xFF;
    _write(K_CUR_2, &portData,1);      //se manda los 4 bits menos significativos del cursor

}


//funcion invocada desde sCreenHandler para hacer scroll de la pantalla
void
upScreen(console_t *console )
{
	int i;

	for (i=(hlines+shlines)*COL*2; i<2*COL*(ROW-1-flines); i++)
		console->vbuffer[i]=console->vbuffer[i+2*COL];
	
	for (;i<2*COL*(ROW-flines);i++)
	{
		console->vbuffer[i++]=' ';
		console->vbuffer[i]=console->currentColor;
	}

	console->cursor=2*COL*(ROW-1-flines);

}


//funcion invocada desde sCreenHandler para hacer un salto de l�nea
void newLine( console_t *console )
{
	if (console->cursor>=(ROW-1-flines)*2*COL)	
		upScreen(console);
	else
		console->cursor+=2*COL-console->cursor%(2*COL);

} 



//cambia el atributo de los caracteres que se imprimen en pantalla
void
setColor( colorType color )
{
	console_t *console = getProcessConsole();

	console->currentColor=color;
 
}

//reinicia la pantalla, pero sin borrar lo que estaba
void updateScreen( void ) 
{
	console_t *console = getProcessConsole();
	
	console->cursor=(hlines+shlines)*COL*2;
}

//clear
void clearScreen( void ) 
{
	sCreenHandler( "",0, MAIN_SCREEN,0 ); //
}


//esta funcion es incovada desde la int 9h, para guardar en buffer las teclas tipeadas
void
guardaBuffer( char Ascii )
{

	console_t *console = actualConsole;

	if (console->kcur==MAX_LINE-1)
	{
		Ascii=ENTER;
		putc('\n');
	}
	
	switch (Ascii)
	{
		case ENTER:
			console->kbuffer[console->kcur++]='\0';
			console->kfull=1;
			break;
		
		case BACK_SPACE:
			if (console->kcur>0)
				--(console->kcur);
			break;
		default:	
			console->kbuffer[console->kcur++]=Ascii;
			break;	
	}

}


