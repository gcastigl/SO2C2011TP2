#ifndef _VIDEO_H
#define _VIDEO_H

#include "stdio.h"
#include "time.h"
#include "process.h"
#include "console.h"
#include "libc.h"


#define COL 80
#define ROW 25

#define CLOCKLEN 34

/** SCREEN COLORS **/
#define WHITE_TXT	0x07
#define YELLOW_TXT 	0x06
#define MAGENTA_TXT 	0x05
#define RED_TXT 	0x04
#define GREENBLUE_TXT	0x03
#define GREEN_TXT 	0x02
#define BLUE_TXT 	0x01
#define BLACK_TXT 	0x00
#define HELP_TXT	YELLOW_TXT

#define TTY_ON_TXT	0x7C
#define TTY_OFF_TXT	0x73

#define TITLE_TXT	0x67
#define TITLE_WIDTH 	78

#define HEAD_COLOR	0x6F
#define HEAD_COLOR_2	0x67
#define SUB_HEAD_COLOR	0x74
#define FOOT_COLOR	0x70
#define RED_FOOT 	0x75
#define YELLOW_FOOT 	0x76
#define MAGENTA_FOOT 	0x75

enum { HEAD_SCREEN,HEAD_TITLE,SUB_HEAD_SCREEN, MAIN_SCREEN, FOOT_SCREEN, FOOT_TTY_F, FOOT_TTY_B };
enum { TTY1,TTY2,TTY3 };

typedef int colorType;



void setColor(colorType color);
//reinicia la pantalla, pero sin borrar lo que estaba
void updateScreen( void );
void sCreenHandler(char *buffer,int count,int screen_sector,int flag);


void flushBuffer(void);
void clearScreen(void);

void init_head ( char *title, int len );
void init_foot ( char *title, int len );

void showTime( char dBte );
//void showTime( time_t *time, char separator, char where, int position );
//void showDate( date_t *date, char separator, char where, int position );

#endif

