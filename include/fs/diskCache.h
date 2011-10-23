#ifndef DISKCACHE_H_
#define DISKCACHE_H_

#include <driver/ata_disk.h>

#define CACHE_SIZE		3

typedef struct {
	int disk;
	u16int sector;
	u8int contents[512];
	boolean dirty;
	int accessCount;
} cachedSector;

void diskCache_init();

void diskCache_write(int disk, void* msg, int bytes, unsigned short sector, int offset);

void diskCache_read(int disk, void* msg, int bytes, unsigned short sector, int offset);

#endif
