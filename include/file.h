#ifndef FILE_H_
#define FILE_H_

#include "stdio.h"

#define NUM_FILES 128

typedef struct kfile_s {
	int pos;
	char name[128];
	char mode[3];
	char used;
	char *contents;
	unsigned len;
} kFILE;


#endif
