/*
 * Este archivo funciona como un cache de los ultimos sectores mas accedidos al disco duro.
 * Siempre que se consulte un archivo, este es buscado en la lista de sectores cacheados,
 * Si existe => se copia los contenidos
 * Si No exite, => se lo cachea y se copian los contenidos.
 */
#ifndef DISKCACHE_H_
#define DISKCACHE_H_

#include <driver/ata_disk.h>
#include <lib/stdlib.h>

#define TICKS_PER_FLUSH	100
#define CACHE_SIZE		10

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

void cache_flush();

void cache_get(int index, cachedSector* cache);

#endif
