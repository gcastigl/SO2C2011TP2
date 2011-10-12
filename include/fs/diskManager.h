#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <driver/ata_disk.h>

#define FILES_INITIAL_SECTOR	1024

#define MAX_NAME_LENGTH			32
#define N_FILE_PAGES			250
#define FILE_BLOCK_SIZE_BYTES	200

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
	int sector;
	u32int offset;
	u32int totalLength;
} FilePage;

typedef struct {
	u32int magic;
	u32int length;
	u32int nextSector;
	u32int nextOffset;
} FileHeader;

void write_header();

boolean validate_header();

#endif
