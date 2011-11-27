#ifndef _SHELL_H
#define _SHELL_H

#include "stdio.h"
#include "libc.h"
#include "video.h"
#include "time.h"


#define PROMPT "user@tp:>"

enum {HELP,CLEAR,SET_KWD,SHOW_PCI,INVAL,WPF,ME,KILLALL,KILL,TOP,CHP,PARENT,GROWSTACK,SHOWSTACK,PRINTSTACK,PROCPRIORITY,EMPTY};

int shell(int argc,char **argv);
void begin();
int readComand(int * opBack);
#endif
