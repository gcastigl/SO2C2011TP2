#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <driver/ata_disk.h>

#define FILES_INITIAL_SECTOR			1024

#define MAX_NAME_LENGTH					32
#define MAGIC_NUMBER					123456
#define FILE_BLOCK_OVERHEAD_SIZE_BYTES	(sizeof(DiskPage) + sizeof(FileHeader))

#define FILE_INITIAL_SIZE_BYTES			(FILE_BLOCK_OVERHEAD_SIZE_BYTES + 100)

#define DISK_BLOCK_SIZE_BYTES			200


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
	u32int magic;
	u32int disk;
	u32int nextSector;
	u32int nextOffset;
	u32int totalLength;
	u32int usedBytes;
	boolean hasNextPage;
} DiskPage;

typedef struct {
	DiskPage data;
	u32int contentUsedBytes;
	u32int contentMaxBytes;
	u32int totalReservedMem;			// Suma del tamaño total de cada pagina
	u32int usedMem;						// Suma del tamaño usando por cada pagina
} iNodeDisk;

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

void diskManager_createInode(iNode* inode, u32int inodeNumber, char* name);
void diskManager_readInode(iNode *inode, u32int inodeNumber, char* name);

int diskManager_writeContents(u32int inodeNumber, char* contents, u32int length, u32int offset);
int diskManager_readContents(u32int inodeNumber, char* contents, u32int length, u32int offset);


void diskManager_getFileName(u32int inode, char* name);

void diskManager_setFileName(u32int inode, char* name);

u32int diskManager_size(u32int inodeNumber);

#endif
