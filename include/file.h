#ifndef FILE_H_
#define FILE_H_

#include <lib/stdio.h>

#define NUM_FILES 128

typedef struct kfile_s {
	int pos;
	char name[128];
	char mode[3];
	char used;
	char *contents;
	unsigned len;
} kFILE;


/*
void createFilesystem();

int loadFileSystem();

int fileIO(char * a);

int pwd(char * arg);

int cat(char * arg);

int cd(char * arg);

int touch( char * arg );

int mkdir(char * arg);

int ls(char * a);

int edit(char * arg);

int tree(char *a);

int rm(char * arg);
*/
#endif
