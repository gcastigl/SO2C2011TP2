 #ifndef _TIME_H
#define _TIME_H

#include "libc.h"   // definiciones en puertos de entrada salida 
#include "video.h"  // showTime
#include "libasm.h" // _write _read

typedef struct date{
    char day, month, year;
} date_t;

typedef struct time{
  char hour, minutes, seconds;
} time_t;

typedef struct crono {
  date_t date;
  time_t time;
} crono_t;

crono_t getTime ( void ); // get time machine from Real Time Clock

#endif
