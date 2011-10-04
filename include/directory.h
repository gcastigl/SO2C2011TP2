#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <defs.h>
#include <lib/kheap.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define MAX_FILENAME_LENGTH		32
#define MAX_FILES_PER_FOLDER	32
#define MAX_FOLDERS_PER_FOLDER	32

typedef struct {
	char name[MAX_FILENAME_LENGTH];
	u32int sector;
	u32int offset;
	char* contents;
	u32int contentsLength;
	u32int used;			// if true, it means the file need to be written to disk
	//struct Directory* folder;
} iNode;

typedef struct Directory {
	char name[MAX_FILENAME_LENGTH];
	u32int filesCount;
	struct Directory* parent;
	u32int subDirsCount;
	struct Directory* subDirs[MAX_FOLDERS_PER_FOLDER];
} Directory_t;

typedef struct {
	Directory_t* dir;
	iNode* files[MAX_FILES_PER_FOLDER];
} fileTable;

void directory_initialize();

int directory_createDir(Directory_t* parent, char* name);

int directory_createFile(Directory_t* dir, char* name);

Directory_t* directory_get(Directory_t* dir, char* name);

Directory_t* directory_getRoot();

void directory_setRoot(Directory_t* dir);

boolean directory_exists(Directory_t* dir, char* name);

Directory_t* directory_find(Directory_t* from, char* name);

#endif
