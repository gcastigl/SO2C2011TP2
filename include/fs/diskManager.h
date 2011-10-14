#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <driver/ata_disk.h>

#define FILES_INITIAL_SECTOR	1024

#define MAX_NAME_LENGTH			32
#define N_FILE_PAGES			250
#define FILE_BLOCK_SIZE_BYTES	200

#define MAGIC_NUMBER			123456

#define MODE_ALL_CONTENTS	-1
#define MODE_NO_CONTENTS	0

typedef struct {
	u32int sector;
	u32int offset;
	u32int length;
	char* contents;
} iNode;

typedef struct {
	u32int magic;
	u32int totalNodes;
	u32int maxNodes;
} FSHeader;

typedef struct {
	u32int sector;
	u32int offset;
	u32int totalLength;
	boolean isDeleted;
} FilePage;

typedef struct {
	u32int magic;
	u32int usedBytes;
	u32int maxBytes;
	u32int nextSector;
	u32int nextOffset;
	boolean hasNextPage;
} FileHeader;

void diskManager_init();

boolean diskManager_validateHeader();

void diskManager_writeHeader();

int diskManager_createiNode();

void diskManager_updateiNodeContents(u32int inodeNumber, char* contents, u32int length);

int diskManager_readiNode(iNode* inode, int inodeNumber, int mode);

#endif
