#ifndef FILE_H
#define FILE_H

#include <defs.h>
#include <lib/stdio.h>
#include <fs.h>
#include <tty.h>

typedef struct {
	u32int index;
} FILE;

int cd(int argc, char *argv[]);

int ls(int argc, char *argv[]);

int mkdir(int argc, char *argv[]);

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
