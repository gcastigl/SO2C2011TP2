#include "../include/console.h"


console_t consoles[MAX_CONSOLES]={0};
console_t *actualConsole=&consoles[0];
// video.c
extern int hlines;
extern int shlines;
extern int flines;

extern char *video;
extern char *foot_time;
extern int currentPID;


void
setupConsoles(void)
{
	int i;

	int timecur=2*(COL*(ROW-flines)+63);

	for (i=0;i<MAX_CONSOLES;i++)
	{
	    consoles[i].tty=i+1;
	    consoles[i].currentColor=WHITE_TXT;
	    consoles[i].sheadcur=hlines*2*COL;
	    consoles[i].cursor=(shlines+hlines)*2*COL;
	    consoles[i].footcur=2*COL*(ROW-flines);
	    consoles[i].ttycur=consoles[i].footcur+6*2;
	}
	foot_time=video+timecur;
}


//Retorna el numero de tty actual
int
getProcessTty(void)
{
    PROCESST *proc = getProcessByPID(currentPID);
  
    return proc->tty;
}

//Retorna el puntero a la consola actual
console_t *
getProcessConsole(void)
{
  return &consoles[getProcessTty()-1];
}

void
printActiveConsole(int tty)
{
	char * ttys[MAX_CONSOLES]={"1","2","3","4","5"};
	int i;

	for (i=1;i<tty;i++)
	  gputs( FOOT_TTY_B, ttys[i-1]);

	  gputs( FOOT_TTY_F, ttys[tty-1] );

	for (i=tty+1;i<=MAX_CONSOLES;i++)
	  gputs( FOOT_TTY_B, ttys[i-1]);
}

