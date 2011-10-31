#include <fs/diskCache.h>

#define MIN(x,y)	((x) < (y) ? (x) : (y))

PRIVATE int _cacheIndex(int disk, short sector);
PRIVATE int _nextFreeIndex();
PRIVATE void _cachSector(int index, int disk, short sector);

static cachedSector cachedData[CACHE_SIZE];

void diskCache_init() {
	log(L_DEBUG, "Initializing disk cache....");
	int i;
	for (i = 0; i < CACHE_SIZE; i++) {
		cachedData[i].accessCount = -1;
	}
}

void diskCache_read(int disk, void* msg, int bytes, unsigned short sector, int offset) {
	//ata_read(disk, msg, bytes, sector, offset);
	//return;
	ata_normalize(&sector, &offset);
	//	log(L_DEBUG, "cache READ -> [%d, %d] bytes: %d", sector, offset, bytes);
	int index = _cacheIndex(disk, sector);
	if (index == -1) {
		index = _nextFreeIndex();
		_cachSector(index, disk, sector);
	}
	cachedSector* cache = &cachedData[index];
	if (offset + bytes >= SECTOR_SIZE) {
		int bytesFromFisrtSector = SECTOR_SIZE - offset;
		//	log(L_DEBUG, "%d - recursive call with: %d, ->%d, %d, %d, %d", msg, cache->disk, bytesFromFisrtSector, bytes - bytesFromFisrtSector, sector + 1, 0);
		memcpy(msg, cache->contents + offset, bytesFromFisrtSector);
		diskCache_read(cache->disk, msg + bytesFromFisrtSector, bytes - bytesFromFisrtSector, sector + 1, 0);
		cache->accessCount++;
		return;
	}
	cache->accessCount++;
	memcpy(msg, cache->contents + offset, bytes);
}

void diskCache_write(int disk, void* msg, int bytes, unsigned short sector, int offset) {
	//ata_write(disk, msg, bytes, sector, offset);
	//return;
	ata_normalize(&sector, &offset);
	//	log(L_DEBUG, "cache WRITE -> [%d, %d] bytes: %d", sector, offset, bytes);
	int index = _cacheIndex(disk, sector);
	if (index == -1) {
		index = _nextFreeIndex();
		_cachSector(index, disk, sector);
	}
	cachedSector* cache = &cachedData[index];
	cache->dirty = true;
	cache->accessCount++;
	if (offset + bytes >= SECTOR_SIZE) {
		int bytesFromFisrtSector = SECTOR_SIZE - offset;
		memcpy(cache->contents + offset, msg, bytesFromFisrtSector);
		//	log(L_DEBUG, "recursive call with: %d, ->%d, %d, %d, %d", cache->disk, bytesFromFisrtSector, bytes - bytesFromFisrtSector, sector + 1, 0);
		diskCache_write(cache->disk, msg + bytesFromFisrtSector, bytes - bytesFromFisrtSector, sector + 1, 0);
		return;
	}
	// log(L_DEBUG, "writing %d bytes to %d, %d", bytes, cache->sector, offset);
	memcpy(cache->contents + offset, msg, bytes);
}

/*
 * Busca el indice del array en el que se encuentra cacheado el sector "sector" del disco "disk"
 */
PRIVATE int _cacheIndex(int disk, short sector) {
	// log(L_DEBUG, "searching for disk: %d, sector: %d", disk, sector);
	int i;
	for (i = 0; i < CACHE_SIZE; i++) {
		if (cachedData[i].accessCount < 0 && cachedData[i].accessCount != -1) {
			log(L_ERROR, "one sector has value %d", cachedData[i].accessCount);
		}
			// log(L_DEBUG, "\t%d -> accessCount : %d, cachedSector: %d, disk: %d", i, cachedData[i].accessCount, cachedData[i].sector, cachedData[i].disk);
		if (cachedData[i].accessCount != -1 &&
			cachedData[i].sector == sector &&
			cachedData[i].disk == disk) {
			return i;
		}
	}
	// log(L_DEBUG, "not found...");
	return -1;
}

/*
 * Busca la siguiente posicion del array para guardar nuevos contenidos
 */
PRIVATE int _nextFreeIndex() {
	int i, nextTofree = 0;
	for(i = 0; i < CACHE_SIZE; i++) {
		if (cachedData[i].accessCount == -1) {
			return i;
		} else if (cachedData[i].accessCount < cachedData[nextTofree].accessCount) {
			nextTofree = i;
		}
	}
	if (cachedData[nextTofree].dirty) {
	//	log(L_DEBUG, "writing sector %d", cachedData[nextTofree].sector);
		ata_write(cachedData[nextTofree].disk, cachedData[nextTofree].contents, SECTOR_SIZE, cachedData[nextTofree].sector, 0);
	}
	return nextTofree;
}

/*
 * Cachea el sector "sector" del diso "disk" en el indice "index"
 */
PRIVATE void _cachSector(int index, int disk, short sector) {
	cachedData[index].disk = disk;
	cachedData[index].sector = sector;
	cachedData[index].accessCount = 1;
	cachedData[index].dirty = false;
	ata_read(disk, cachedData[index].contents, SECTOR_SIZE, sector, 0);
	// log(L_DEBUG, "Catching %d -> [%d, %d]", index, sector, 0);
}

void cache_flush() {
	int i;
	for(i = 0; i < CACHE_SIZE; i++) {
		if (cachedData[i].dirty) {
			ata_write(cachedData[i].disk, cachedData[i].contents, SECTOR_SIZE, cachedData[i].sector, 0);
			cachedData[i].dirty = false;
		}
	}
}

