#include "../include/libc.h"

#define EXEC_CODE   1
#define KILL_CODE   2
#define SLEEP_CODE   3

extern console_t *actualConsole;
extern console_t consoles[];
extern int currentPID;

char 	shiftStatus=OFF;
char 	capsStatus=OFF;
int 	acentoStatus=OFF;
int 	specialKey=OFF;
int 	altGrStatus=OFF;
int	dieresisStatus=OFF;

char * video=(char*)VIDEO;
char * foot_time=0;

char table [][NUM_KEYS][2]={	//Espanol
			{{ERROR,ERROR},{ESC,ESC},{'1','!'},{'2','"'},{'3','#'},{'4','$'},{'5','%'},{'6','&'},{'7','/'},{'8','('},
			{'9',')' },{'0','='},{'\'','?'},{SIG_PREG,SIG_EXC},{BACK_SPACE,BACK_SPACE}, {TAB,TAB},{'q','Q'},{'w','W'},{'e','E'},{'r','R'},
			{'t','T'},{'y','Y'},{'u','U'},{'i','I'},{'o','O'},{'p','P'},{ACENTO,ACENTO},{'+','*'}, {ENTER,ENTER}, {CTRL,CTRL},
			{'a','A'},{'s','S'},{'d','D'},{'f','F'},{'g','G'},{'h','H'},{'j','J'},{'k','K'},{'l','L'},{ENIEMINUS,ENIEMAYUS},
			{'{','['},{'|',O_},{L_SHIFT,L_SHIFT}, {'}',']'}, {'z','Z'}, {'x','X'}, {'c','C'}, {'v','V'}, {'b','B'}, {'n','N'}, 
			{'m','M'},{',',';'},{'.',':'},{'-','_'}, {R_SHIFT,R_SHIFT}, {PT_SCROLL,PT_SCROLL}, {ALT,ALT}, {SPACE,SPACE}, {CPSLK,CPSLK}, {F1 ,F1},
			{F2,F2}, {F3 ,F3}, {F4,F4}, {F5,F5}, {F6,F6}, {F7,F7}, {F8,F8},
			{F9,F9},{F10,F10},{NUM_LK,NUM_LK},
			{SCRL_LK,SCRL_LK}, {'7',HOME}, {'8',UP_ARROW}, {'9',PG_UP}, {'-','-'}, {'4','4'}, {'5','5'}, {'6','6' }, {'+','+'},{'1','1'},
			{'2','2'}, {'3','3'}, {'0','0'}, {'.','.'}, {NOPRINT,NOPRINT}, {NOPRINT,NOPRINT}, {NOPRINT,NOPRINT}, {F11,F11},{F12,F12}}, 	
			//Ingles
			{{ERROR,ERROR},{ESC,ESC},{'1','!'},{'2','@'},{'3','#'},{'4','$'},{'5','%'},{'6','^'},{'7','&'},{'8','*'},
			{'9','(' },{'0',')'},{'-','_'},{'=','+'},{BACK_SPACE,BACK_SPACE}, {TAB,TAB},{'q','Q'},{'w','W'},{'e','E'},{'r','R'},
			{'t','T'},{'y','Y'},{'u','U'},{'i','I'},{'o','O'},{'p','P'},{'[','{'},{']','}'}, {ENTER,ENTER}, {CTRL,CTRL},
			{'a','A'},{'s','S'},{'d','D'},{'f','F'},{'g','G'},{'h','H'},{'j','J'},{'k','K'},{'l','L'},{';',':'},
			{'\'','"'},{'`','~'},{L_SHIFT,L_SHIFT}, {'\\','|'},{'z','Z'},{'x','X'},{'c','C'},{'v','V'},{'b','B'},{'n','N'},
			{'m','M'},{',','<'},{'.','>'},{'/','?'}, {R_SHIFT,R_SHIFT}, {PT_SCROLL,PT_SCROLL}, {ALT,ALT},{SPACE,SPACE},{CPSLK,CPSLK},{F1,F1},
			{F2,F2},{F3,F3},{F4,F4},{F5,F5},{F6,F6},{F7,F7},{F8,F8},{F9,F9},{F10,F10},{NUM_LK,NUM_LK},
			{SCRL_LK,SCRL_LK},{'7','7'},{'8','8'},{'9','9'},{'-','-'},{'4','4'},{'5','5'},{'6','6' },{'+','+'}, {'1','1'},
			{'2','2'},{'3','3'},{'0','0'},{'.','.'},{NOPRINT,NOPRINT},{NOPRINT,NOPRINT},{NOPRINT,NOPRINT},{F11,F11},{F12,F12}}                                                          	
};



/***************************************************************
*	setup_IDT_entry
*	Inicializa un descriptor de la IDT
*
*	Recibe: Puntero a elemento de la IDT
*	Selector a cargar en el descriptor de interrupcion
*	Puntero a rutina de atencion de interrupcion	
*	Derechos de acceso del segmento
*	Cero
****************************************************************/

void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access, byte cero) 
{
  item->selector = selector;
  item->offset_l = offset & 0xFFFF;
  item->offset_h = offset >> 16;
  item->access = access;
  item->cero = cero;
}


//funci�n auxiliar que utiliza el handler de la interrupci�n 9h
void
int_09(void)
{
	char sCode,Ascii;
	console_t *console=actualConsole;
	PROCESST *procOld, *procNew;

	_read(KWD,&sCode,1);

	Ascii=convertScode(sCode); //procesa el scancode

	if (Ascii>=1 && Ascii<=5)
	{
	    if (procOld=getParentProcessByTty(console->tty)) //si existe bloquea el shell viejo y desbloquea el shell nuevo
		blockProcess(procOld->pid,BLOCKED,NOTBLOCKED);

	    if (procNew=getParentProcessByTty(Ascii))
	    {
		unblockProcessFrom(procNew->pid,BLOCKED);
		actualConsole=&consoles[Ascii-1];
		flushBuffer();
	    }
	}
	else if (Ascii)  //si es un caracter imprimible
	{
	    sCreenHandler(&Ascii,1,MAIN_SCREEN,1);

	    guardaBuffer(Ascii);
	}
}

//si count = 0, significa q hay q hacer un cls
void
int_80(int systemCall,int fd,char *buffer,int count)
{
	int i;

	switch (systemCall)
	{
		case READ:
			switch (fd)
			{
				case KWD:   //teclado
					_in(buffer,KWD_AD,count);
					break;
				case PCI_D:
					_in(buffer,PCI_DATA,count);
					break;
				case PCI_I:
					_in(buffer,PCI_INDEX,count);
					break;
				case USB:
					break;
				default:
					_in(buffer,fd,count);
					break;
			}
			break;
		
		case WRITE:
			switch (fd)
			{
				case STDOUT:
					for (i=0;i<count;i++)
						video[i]=buffer[i];
					break;
				case FOOT_TIME:
					for (i=0;i<count;i++)
						foot_time[i]=buffer[i];
					break;
				case PCI_D:
					_out(PCI_DATA, buffer,count);
					break;
				case PCI_I:
					_out(PCI_INDEX, buffer,count);
					break;
				case PIC:
					_out(PIC_AD,buffer,count);
					break;
				case K_CUR_1:
					_out(KWD_CUR_1,buffer,count);
					break;	
				case K_CUR_2:
					_out(KWD_CUR_2,buffer,count);
					break;
				default:
					_out(fd,buffer,count);
			}	
			break;
	}
}

void
int_85 (int code, void *param1, void *param2, void *param3) 
{
   	 switch (code) 
	{
		case EXEC_CODE:
			execute (param1, param2, (int) param3);
			break;
		case KILL_CODE:
			Kill ((PROCESST*)param1);
			break;
		case SLEEP_CODE:
			sleep ((int)param1);
			break;
    	}
    return;
}


//retorna si es una letra o no
int
isAlpha_(unsigned char  sCode)
{
	return ((sCode>=SCODE_Q && sCode<=SCODE_P) || (sCode >=SCODE_A && sCode<=SCODE_L) || (sCode>=SCODE_Z && sCode<=SCODE_M));
}


//retorna si es vocal o no
int
isVowel_(unsigned char sCode)
{
	return (sCode == SCODE_A || sCode == SCODE_E || sCode == SCODE_I || sCode == SCODE_O || sCode == SCODE_U);
}

//funci�n que devuelve el acii de las letras vocales con acentos
char
vocalAcentuada(unsigned char sCode)
{
	char ascii;

	switch(sCode)
	{
		case SCODE_A : ascii=a_ACENT; break; 
		case SCODE_E : 
			if ((capsStatus==ON && shiftStatus==OFF) || (capsStatus==OFF && shiftStatus==ON))
				ascii=E_ACENT;
			else
				ascii=e_ACENT;break;
		case SCODE_I : ascii=i_ACENT; break;
		case SCODE_O : ascii=o_ACENT; break;
		case SCODE_U : ascii=u_ACENT; break;
	}

	return ascii;
}


//funci�n que devuelve el acii de las letras vocales con di�resis
char
vocalDieresis (unsigned char sCode)
{
	char ascii;
	
	switch(sCode)
	{
		case SCODE_A : 	ascii=a_DIERE;break; 
		case SCODE_E :	ascii=e_DIERE;break;
		case SCODE_I : 	ascii=i_DIERE;break;
		case SCODE_O : 	ascii=o_DIERE;break;
		case SCODE_U : 	ascii=u_DIERE;break;
	}

	return ascii;
	
}

//funci�n que procesa los scancodes que comienzan con 0xE0
char
convertSpKeys(unsigned char sCode)
{
	char ascii;
	
	switch(sCode)
	{
		case SCODE_DEL : 	ascii= SIMB1; break;
		case SCODE_DOWNARR : 	ascii= SIMB2; break;
		case SCODE_END : 	ascii= SIMB1; break;
		case SCODE_HOME : 	ascii= SIMB1; break;
		case SCODE_INS : 	ascii= SIMB1; break;
		case SCODE_LEFTARR : 	ascii= SIMB2; break;
		case SCODE_PGDN : 	ascii= SIMB1; break;	
		case SCODE_PGUP : 	ascii= SIMB1; break;
		case SCODE_RIGHTARR : 	ascii= SIMB2; break;
		case SCODE_UPARR : 	ascii= SIMB2; break;
		case SCODE_RIGHTALT : 	{	
						if(language == SPANISH)
							altGrStatus=ON;
						ascii= 0;
					}break;
		case SCODE_RIGHTCTRL:	ascii=SIMB3 ; break;
		case SCODE_BAR: 	ascii='/'; break;
		case SCODE_ENTER: 	ascii='\n';break;
		default : 	 	ascii= 0; break;
	}	

	return ascii;
}

char
convertAltGr(unsigned char sCode)
{
	char ascii;

	switch(sCode)
	{		
		case SCODE_Q : 		ascii='@'; break;
		case SCODE_COMILLA: 	ascii='\\'; break;
		case SCODE_CIRC: 	ascii= BARRA;break;
		case SCODE_MAS:		ascii= '~'; break;
		case SCODE_LLAVEABRE: 	ascii='^'; break;
		case SCODE_LLAVECIERRA: ascii='`'; break;
		default: ascii=0; break;		
	}

	return ascii;
}

//retorna 0 si no es un caracter imprimible
//o el ascii q corresponda.
//ascii de 1 a 5 identifican a las teclas F1 a F5

char
convertScode(unsigned char sCode)
{
	char Ascii=0;
	int state;
	char portData;

	if(specialKey)
	{
		Ascii=convertSpKeys(sCode);	
		specialKey=OFF;
	}
	else
	{
		if ( sCode<= NUM_KEYS)	//si no es breakCode
		{	
			if( sCode == MKC_LSHIFT || sCode == MKC_RSHIFT )		
				shiftStatus=ON;
			else if (sCode==MKC_CAPS)
			{
				capsStatus=!capsStatus; //modifica el estado del caps
			}
			else if (sCode == SCODE_ACENTO && language == SPANISH)
			{
				if(shiftStatus ==ON)
					dieresisStatus=ON;
				else
					acentoStatus=ON;
			}
      else if(sCode>=SCODE_F1 && sCode<=SCODE_F5)
          Ascii=(char)(sCode-SCODE_F1+1);

			else 
			{	
				if( altGrStatus )
				{
					Ascii=convertAltGr(sCode);
					altGrStatus =OFF;
				}
				else if( acentoStatus )
				{
					if(isVowel_(sCode))
						Ascii=vocalAcentuada(sCode); 

					acentoStatus=OFF;
				}	 //0x90 es la E mayuscula acentuada
				else if ( dieresisStatus )
				{
					Ascii=vocalDieresis(sCode);
					dieresisStatus=OFF;
				}
				else 
				{	 	
					if(isAlpha_(sCode))
						state= ((capsStatus==ON && shiftStatus==OFF) || (capsStatus==OFF && shiftStatus==ON));
					else	
						state= shiftStatus==ON;
			
					Ascii=table[language][sCode][state];	 
				}
			}
		}
		else if( sCode == BRK_RSHIFT || sCode == BRK_LSHIFT )
			shiftStatus=OFF;
		else if (sCode == SCODE_SPKEY)
			specialKey=ON;		
	}
	
	return Ascii;
}



