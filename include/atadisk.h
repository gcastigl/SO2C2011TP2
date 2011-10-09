#ifndef ATADISK_H_
#define ATADISK_H_

#include <defs.h>
#include <fs.h>
#include <driver/ata_disk.h>
#include <driver/video.h>

#define INODES					50
#define FILE_BLOCK_SIZE_BYTES	200
#define MAX_NAME_LENGTH			32

#define MAGIC_NUMBER			123456

typedef struct {
	u32int sector;
	u32int offset;
	u32int length;
	char* contents;
	int contentsSize;
} iNode;

typedef struct {
	u32int magic;
	u32int totalNodes;
} FSHeader;

typedef struct {
	u32int sector;
	u32int offset;
} FilePage;

typedef struct {
	u32int magic;
	u32int length;
	u32int nextSector;
	u32int nextOffset;
} FileHeader;

void atadisk_init();

fs_node_t* atadisk_getRoot();

#endif
