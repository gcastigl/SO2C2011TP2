#include "../include/time.h"


#define TIME_POS 63
#define DATE_POS 63

//extern console_t *console;

crono_t 
getTime ( void )
{
	crono_t crono={0};
	char index=0;
	char timeStr[20]={0};

	do{
	index=10; 			//;Get RTC register A
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );  
	} while ( index & 0x80 ); 	// Is update in progress?

	index=0;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.time.seconds=index;

	index=2;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.time.minutes=index;

	index=4;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.time.hour=index;

	index=7;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.date.day=index;

	index=8;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.date.month=index;

	index=9;
	_write( RTC_INDEX,&index,1 );
	_read ( RTC_DATA, &index,1 );
	crono.date.year=index;

	gprintHex( crono.date.day, FOOT_TIME );
	gputc('/');
	gprintHex( crono.date.month, FOOT_TIME );
	gputc('/');
	gprintHex( crono.date.year, FOOT_TIME );
	gputc(' ');

	gprintHex( crono.time.hour, FOOT_TIME );
	gputc(':');
	gprintHex( crono.time.minutes, FOOT_TIME );
	gputc(':');
	gprintHex( crono.time.seconds, FOOT_TIME );

	return crono;
}

