#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <driver/ata_disk.h>

#define FILES_INITIAL_SECTOR	1024

#define MAX_NAME_LENGTH			32
#define N_FILE_PAGES			250
#define FILE_BLOCK_SIZE_BYTES	200

#define MAGIC_NUMBER			123456

typedef struct {
	// fields required by posix, this is still missing some fields
	u32int sector;
	u32int offset;
    u32int inodeId;        			// Identifies the structure
    u32int uid;        				// The owning user.
    u32int gid;         			// The owning group.
    u32int flags;       			// Includes the node type.
    u32int length;      			// Size of the file, in bytes.
    u32int impl;        			// An implementation-defined number.
    u32int mask;        			// The permissions mask.
} iNode;

typedef struct {
	u32int magic;
	u32int totalNodes;
	u32int maxNodes;
} FSHeader;

typedef struct {
	u32int totalLength;
	u32int magic;
	u32int nextSector;
	u32int nextOffset;
	u32int usedBytes;
	boolean hasNextPage;
} FilePage;

typedef struct {
	u32int magic;
	char name[MAX_NAME_LENGTH];
    u32int mask;        			// The permissions mask.
    u32int uid;        				// The owning user.
    u32int gid;         			// The owning group.
    u32int flags;       			// Includes the node type. See #defines above.
    u32int impl;        			// An implementation-defined number.
} FileHeader;

void diskManager_init();

boolean diskManager_validateHeader();

void diskManager_writeHeader();

int diskManager_nextInode();

void diskManager_writeContents(iNode* inode, char* contents, u32int length);

void diskManager_readiNode(iNode *inode, int inodeNumber);

void diskManager_getFileName(u32int inode, char* name);

void diskManager_setFileName(u32int inode, char* name);

char *diskManager_readContents(u32int inodeNumber, int* length);

#endif
