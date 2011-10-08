#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <defs.h>
#include <lib/kheap.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define FILE_TABLE_SIZE			64
#define MAX_FILENAME_LENGTH		32
#define MAX_FILES_PER_FOLDER	32
#define MAX_FOLDERS_PER_FOLDER	32

typedef struct {
	char name[MAX_FILENAME_LENGTH];
	u32int sector;
	u32int offset;
	char* contents;
	u32int contentsLength;
	boolean used;			// if true, it means the file need to be written to disk
	//struct Directory* folder;
} iNode;

typedef struct Directory_t {
	char name[MAX_FILENAME_LENGTH];
	struct Directory_t* parent;
	u32int subDirsCount;
	struct Directory_t* subDirs[MAX_FOLDERS_PER_FOLDER];
	struct FileTableEntry_t* fileTableEntry;
} Directory;

typedef struct FileTableEntry_t {
	Directory* dir;
	iNode** files;
	u32int filesCount;
} FileTableEntry;

void directory_initialize();

int directory_createDir(Directory* parent, char* name);

int directory_createFile(Directory* dir, char* name);

Directory* directory_get(Directory* dir, char* name);

Directory* directory_getRoot();

void directory_setRoot(Directory* dir);

boolean directory_exists(Directory* dir, char* name);

Directory* directory_find(Directory* from, char* name);

void initEmptyDirectory(Directory* dir, char* name);

#endif
