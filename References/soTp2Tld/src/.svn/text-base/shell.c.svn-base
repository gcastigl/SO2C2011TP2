
#include "../include/shell.h"

extern char *video;
extern char *foot_time;
extern int hlines;
extern int shlines;
extern int flines;

extern PROCESST processes[];
extern console_t *actualConsole;

//Parametro de kill
char param[MAX_LINE];

int shell(int argc,char **argv)
{
	int cmd_opt;	
	char prompt[]=PROMPT; 
	int opBack;

	language=ENGLISH;
	
	gputs( HEAD_SCREEN, 	" Sistemas Operativos TP 2 - 2009  ::: Multitasker ::: " );
	gputs( SUB_HEAD_SCREEN, " Debrouvier, Liendro & Teisaire " );
	gputs( FOOT_SCREEN, 	" Tty: " );
	

	printActiveConsole(actualConsole->tty);

	do
	{
		setColor(GREENBLUE_TXT);
		puts( prompt );
		setColor(WHITE_TXT);	
	
		cmd_opt=readComand(&opBack);
	
		switch(cmd_opt)
		{
			case HELP:
			    _exec("help",help,opBack); 
			    break;
			case CLEAR:
			    _exec("clear",clear,opBack); 
			    break;
			case SET_KWD:
			    _exec("setKwd",setKwd,opBack); 
			    break;
			case SHOW_PCI:
			    _exec("lspci",showPci,opBack);
			    break;
			case INVAL:
			    setColor(RED_TXT);
			    puts("Comando inv");
			    putc(a_ACENT);
			    puts("lido! ");
			    setColor(WHITE_TXT);
			    puts( "Escriba help para ver comandos\n\n");
			    break;

			case ME:
				_exec("mem_use",mem_use,opBack);
				break;
			
			case CHP:
				_exec("down_fault", down_fault, opBack);
				break;
				
			case PARENT:
				_exec("parentTouch", parentTouch, opBack);
				break;
				
			case GROWSTACK:
				_exec("growStack",growStack,opBack);
				break;
				
			case SHOWSTACK:
			{
			  char * param[3]={"pepe","Juan","Alverto"};
			    
			  createProcess("showStack",showStack,getProcessTty(),3,&(param[0]),STACK_USER,0,BACKGROUND,NOTBLOCKED);
			  //_exec("showStack",showStack,opBack);
			} 
				break;

				
			
			case KILLALL:
			{
				PROCESST * p;
        int flag=0;

				while ((p=getProcessByName(param))!=0)
        {
            flag=1;
				    _kill(p);
				}

        if (!flag)
				    puts("Proceso invalido\n");
				break;
			 }
			case KILL:
			{
				PROCESST * p; 
				int pid;

				pid=atoi(param);

				//printDec_int(pid);
				if (p=getProcessByPID(pid))
				    _kill(p);
				else
				    puts("Proceso invalido\n");
					
				break;
			 }

			case TOP: 
				_exec("top",top,opBack);
				break;
			case PRINTSTACK: 
				 _exec("printStack",printStackPosition,opBack); 
				  break;
			case PROCPRIORITY:
          _Cli();

              int prioridad;
              
              if (*param=='\0' || ( (prioridad=atoi(param))<0 || prioridad >3) )
                   strcpy(param,"3");

              char * argv[4]={"0","1","2","3"};
				      createProcess("priority",priorityTest,getProcessTty(),1,&(argv[0]),STACK_USER,0,BACKGROUND,NOTBLOCKED);

				      createProcess("priority",priorityTest,getProcessTty(),1,&(argv[atoi(param)]),STACK_USER,atoi(param),BACKGROUND,NOTBLOCKED);
          _Sti();

				 break;
			break;
			

		}
	}while(1);

}

int
readComand(int * opBack)
{
	int cmd_opt;
	char line[MAX_LINE];
	char comand[MAX_LINE];	

	int i,j,len;
	char c;

	param[0]='\0';

	getline(line);
	len=strlen(line);		

	for(i=0; i< len && (c=line[i])!= ' ' ;i++)
		comand[i]=c;
	
	if(comand[i-1]=='&')
	{
		*opBack=BACKGROUND;
		comand[i-1]='\0';
	}	
	else
	{
		*opBack=FRONTGROUND;
		comand[i]='\0';
	}	

	if(c==' ') //Almaceno el parametro	
	{
		i++;
		for(j=0; j < len ; j++)
			param[j]=line[i++];
		param[i]='\0';
	}			

	if (strcmp(comand,"help")==0)
		cmd_opt=HELP;
	else if (strcmp(comand,"clear")==0)
		cmd_opt=CLEAR;
	else if (strcmp(comand,"kwd")==0)
		cmd_opt=SET_KWD;
	else if (strcmp(comand,"pci")==0)
		cmd_opt=SHOW_PCI;
	else if (strcmp(comand,"me")==0)
		cmd_opt=ME;
	else if (strcmp(comand,"chp")==0)
		cmd_opt=CHP;
	else if (strcmp(comand,"killall")==0)
		cmd_opt=KILLALL;
	else if (strcmp(comand,"kill")==0)
		cmd_opt=KILL;
	else if (strcmp(comand,"top")==0)
		cmd_opt=TOP;
	else if (strcmp(comand,"parent")==0)
		cmd_opt=PARENT;
	else if (strcmp(comand,"gs")==0)
		cmd_opt=GROWSTACK;	
	else if (strcmp(comand,"ss")==0)
		cmd_opt=SHOWSTACK;	
	else if (strcmp(comand,"printStack")==0)
		cmd_opt=PRINTSTACK;
	else if (strcmp(comand,"priority")==0)
		cmd_opt=PROCPRIORITY;
	else if (*comand=='\0')	//si se escribe enter sin nada en el buffer, no hace nada
		cmd_opt=EMPTY;
	else
		cmd_opt=INVAL;

	return cmd_opt;
}
 
void begin()
{
	setColor(RED_TXT);
	puts( "TRABAJO PRACTICO ESPECIAL 2\n");
	setColor(YELLOW_TXT);
	puts( "Sistemas operativos - 2009\n");
	setColor(WHITE_TXT);
	puts( "Debrouvier Hemilse,Liendro Jose,Teisaire Emmanuel\n\n");
}
 
