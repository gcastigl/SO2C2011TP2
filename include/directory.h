#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <defs.h>
#include <lib/kheap.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define MAX_FILENAME_LENGTH		32
#define MAX_FILES_PER_FOLDER	32
#define MAX_FOLDERS_PER_FOLDER	32

// ERROR CODES
#define	DIR_EXISTS	1
#define DIR_FULL	2

typedef struct {
	char name[MAX_FILENAME_LENGTH];
	u32int sector;
	u32int offset;
	char* contents;
	int length;
} iNode;

typedef struct Directory_t {
	char name[MAX_FILENAME_LENGTH];
	iNode* files[MAX_FILES_PER_FOLDER];
	u32int filesCount;
	struct Directory_t* parent;
	u32int subDirsCount;
	struct Directory_t* subDirs[MAX_FOLDERS_PER_FOLDER];
} Directory_t;

void directory_initialize();

int directory_create(Directory_t* parent, char* name);

Directory_t* directory_get(Directory_t* dir, char* name);

Directory_t* directory_getRoot();

boolean directory_exists(Directory_t* dir, char* name);

#endif
