#include "kc.h"

void bounds(void);
void invop(void);
void div0(void);
void gpf(void);
void ssf(void);
void snp(void);
void setss(void);
void setds(void);
void setcs(void);

void setss()
{
/*	int a;
	int valss;
	char *video = (char*) 0xB8000;
	int *stacksegment = (int*) 0x10000;
	a=lenspace(buffer);
	valss=atoi2(&(buffer[a+1]));
	*stacksegment=valss*8;
	ponerss();*/
}

void setds()
{
	/*int a;
	int valds;
	char *video = (char*) 0xB8000;
	int *datasegment = (int*) 0x10000;
	a=lenspace(buffer);
	valds=atoi2(&(buffer[a+1]));
	*datasegment=valds*8;
	ponerds();*/
}


void bounds()
{
	message("Bounds Exceeded!",0,0);
}

void invop()
{
	message("Invalid OPCODE",0,0);
}

void div0()
{
	message("Division by 0",0,0);
}


void gpf()
{
	message("General Protection Fault",0,0);
}

void ssf()
{
	message("Stack Segment Fault",0,0);
}


void snp()
{
	message("Segment Not Present",0,0);
}
