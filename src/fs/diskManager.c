#include <fs/diskManager.h>
#include <util/logger.h>

#define MIN(x,y)							(((x) < (y)) ? (x) : (y))
#define BIT(n, i)							((n) & (1 << (i)))

/*
 * sector 0 => FSHeader - inodes bitmap
 * sector 1 => inodes
 * sector 10 => diskBitmap
 * sector 11... =>  archivos...
 */
#define INODES_INITIAL_SECTOR			1
#define FILES_BIT_MAP_SECTOR			10
#define FILE_CONTENTS_INITAL_SECTOR		11

PRIVATE void _getiNode(u32int inodeNumber, iNodeDisk *inode);
PRIVATE void _setiNode(u32int inodeNumber, iNodeDisk *inode);

PRIVATE int _reserveMemoryBitMap(DiskPage *page, int size, u32int initialSector, u32int initialOffset);
PRIVATE int _extendMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset);
PRIVATE void _freeMemory(DiskPage *page);

PRIVATE int _readBlock(DiskPage *page, char *contents, u32int length, u32int offset);
PRIVATE int _writeBlock(DiskPage *page, char *contents, u32int length, u32int offsett);

void _setFileheader(u32int inodeNumber, FileHeader *header);
void _getFileheader(u32int inodeNumber, FileHeader *header);

u32int _availableMem(iNodeDisk* inode);

PRIVATE disk_strategy strategy;

void diskManager_init(u32int strategyType) {
	 log(L_DEBUG, "Using block size: %d", DISK_BLOCK_SIZE_BYTES);
	 log(L_DEBUG, "iNodeDisk size: %d", sizeof(iNodeDisk));
	 log(L_DEBUG, "DiskPage size: %d", sizeof(DiskPage));
	 log(L_DEBUG, "FileHeader size: %d\n", sizeof(FileHeader));
	switch(strategyType) {
		case S_DIRECT_ACCESS:
			strategy.write = ata_write;
			strategy.read = ata_read;
			log(L_DEBUG, "Strategy: Direct disk access\n");
			break;
		case S_LRU_CACHE:
			strategy.write = diskCache_write;
			strategy.read = diskCache_read;
			log(L_DEBUG, "Strategy: LRU Cache\n");
			break;
		default:
			strategy.write = ata_write;
			strategy.read = ata_read;
			log(L_DEBUG, "Strategy: Direct disk access\n");
			break;
	}
}

boolean diskManager_validateHeader() {
	FSHeader header;
	strategy.read(ATA0, &header, sizeof(FSHeader), 0, 0);
		log(L_DEBUG, "Header for the OS is %svalid", header.magic == MAGIC_NUMBER ? "" : "NOT ");
	return header.magic == MAGIC_NUMBER;
}

void diskManager_writeHeader() {
	FSHeader header;
	header.magic = MAGIC_NUMBER;
		log(L_DEBUG, "Writing OS header");
	strategy.write(ATA0, &header, sizeof(FSHeader), 0, 0);
	// Mark all sector as free sectors
	char block[SECTOR_SIZE];
	for (int i = 0; i < SECTOR_SIZE; i++) block[i] = '\0';
	// Fill sector 0 with 0s (inodes memory bitmap)
	strategy.write(ATA0, block, SECTOR_SIZE - sizeof(FSHeader), 0, sizeof(FSHeader));
	// Fill sector FILES_BIT_MAP_SECTOR with 0s (file contents memory bitmap)
	strategy.write(ATA0, block, SECTOR_SIZE, FILES_BIT_MAP_SECTOR, 0);
}

int diskManager_nextInode() {
	int numberOfBlocks = SECTOR_SIZE - sizeof(FSHeader);
	char block[numberOfBlocks];
	int next = -1;
	strategy.read(ATA0, block, numberOfBlocks, 0, sizeof(FSHeader));
	for (int i = 0; i < numberOfBlocks && next == -1; i++) {
		for (int j = 0; j < 8 && next == -1; j++) {
			if (BIT(block[i], j) == 0) {
				next = i * 8 + j;
				block[i] |= (1 << j);
			}
		}
	}
	if (next == -1) {
		errno = E_OUT_OF_MEMORY;
		return -1;
	}
	strategy.write(ATA0, block, numberOfBlocks, 0, sizeof(FSHeader));
	//	log(L_DEBUG, "next inode to use: %d", next);
	return next;
}

void diskManager_createInode(iNode* inode, u32int inodeNumber, char* name) {
	iNodeDisk newiNode;
	int initialBlocks = 1;
	int reserved = _reserveMemoryBitMap(&newiNode.data, initialBlocks, FILE_CONTENTS_INITAL_SECTOR, 0);
	if (reserved == -1) {	// Do not set errno because it should already be set
		log(L_ERROR, "failed to reserve memory for inode %s (%d)", name, inodeNumber);
		return;
	}
	newiNode.blocks = initialBlocks;
	newiNode.usedBytes = 0;
	_setiNode(inodeNumber, &newiNode);
		log(L_DEBUG, "Create inode %s(%d) at [%d, %d], bytes used: %d, max bytes: %d, available %d", name, inodeNumber, INODES_INITIAL_SECTOR, \
				inodeNumber * sizeof(iNodeDisk), newiNode.usedBytes, newiNode.blocks * DISK_BLOCK_SIZE_BYTES, _availableMem(&newiNode));
		//log(L_DEBUG, "inode points to: [%d, %d]\n", newiNode.data.nextSector, newiNode.data.nextOffset);
	FileHeader header;
	header.magic = MAGIC_NUMBER;
	strcpy(header.name, name);
	header.gid = inode->gid;
	header.uid  =inode->uid;
	header.flags = inode->flags;
	header.impl = inode->impl;
	header.mask = inode->mask;
	strategy.write(newiNode.data.disk, &header, sizeof(FileHeader), newiNode.data.nextSector, newiNode.data.nextOffset + sizeof(DiskPage));
		// log(L_DEBUG, "written inode header to: [%d, %d + %d]", newiNode.data.nextSector, newiNode.data.nextOffset, sizeof(DiskPage));
}

void diskManager_delete(u32int inodeNumber) {
	// Free used memory by inode
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	log(L_DEBUG, "deleted inode header [%d, %d]", inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
	_freeMemory(&inode.data);

	// Delete header (to avoid errors)
	FileHeader header;
	header.magic = 0;
	_setFileheader(inodeNumber, &header);

	// Mark the inode bit map @inodeNumber as empty
	int numberOfInodes = SECTOR_SIZE - sizeof(FSHeader);
	char inodes[numberOfInodes];
	strategy.read(ATA0, inodes, numberOfInodes, 0, sizeof(FSHeader));
	int pos = inodeNumber / 8;
	int bit = inodeNumber % 8;
	inodes[pos] &= ~(1 << bit);
	strategy.write(ATA0, inodes, numberOfInodes, 0, sizeof(FSHeader));
}

void diskManager_readInode(iNode *inode, u32int inodeNumber) {
	iNodeDisk inodeOnDisk;
	_getiNode(inodeNumber, &inodeOnDisk);
		// log(L_DEBUG, "reading from inode: %d -> [%d, %d]", inodeNumber, inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset);
	if (inodeOnDisk.data.magic != MAGIC_NUMBER) {
			log(L_ERROR, "Can't read iNode %d - Corrupted file!", inodeNumber);
		errno = E_CORRUPTED_FILE;
		return;
	}
	FileHeader header;

	strategy.read(inodeOnDisk.data.disk, &header, sizeof(FileHeader), inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset + sizeof(DiskPage));
	if (header.magic != MAGIC_NUMBER) {
			log(L_ERROR, "Can't read file header (%d) - Corrupted->[%d, %d]", inodeNumber, inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset);
		errno = E_CORRUPTED_FILE;
		return;
	}
	strcpy(inode->name, header.name);
	inode->gid = header.gid;
	inode->uid = header.uid;
	inode->flags = header.flags;
	inode->impl = header.impl;
	inode->mask = header.mask;

	inode->inodeId = inodeNumber;
	inode->length = inodeOnDisk.usedBytes;
	inode->sector = inodeOnDisk.data.nextSector;
	inode->offset = inodeOnDisk.data.nextOffset;
}

void diskManager_writeInode(iNode *inode, u32int inodeNumber) {
	FileHeader header;
	_getFileheader(inodeNumber, &header);
	header.flags = inode->flags;
	header.impl = inode->impl;
	header.mask = inode->mask;
	strcpy(header.name, inode->name);
	_setFileheader(inodeNumber, &header);
}

int diskManager_writeContents(u32int inodeNumber, char *contents, u32int length, u32int offset) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	//	log(L_DEBUG, "updating contents, %d + %d bytes to inode: %d -> [%d, %d]", offset, length, inodeNumber, inode.data.nextSector, inode.data.nextOffset);
	//	log(L_DEBUG,"Contents[MAX: %d, used: %d], reserved blocks = %d", _availableMem(&inode), inode.usedBytes, inode.blocks);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to write to a corrupted page!");
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	if (offset + length > _availableMem(&inode)) {
		log(L_TRACE, "%d -> Memory is not enough, have: %d, extending memory to %d bytes", inodeNumber, _availableMem(&inode), offset + length);
		int extrablocks = _extendMemory(&inode.data, offset + length, FILE_CONTENTS_INITAL_SECTOR, 0);
		if (extrablocks != -1) {
		    inode.blocks += extrablocks;
		} else {
		    log(L_ERROR, "There was an error reserving blocks!");
		    return 0;
		}
	}
	if (offset + length > inode.usedBytes) {
		inode.usedBytes = offset + length;
	}
	int status = _writeBlock(&inode.data, contents, length, offset);
	if (status == -1) {
		log(L_ERROR, "\n_writeBlock has failed writing contents...\n");
		return -1;
	}
	_setiNode(inodeNumber, &inode);
	return 0;
}

int diskManager_readContents(u32int inodeNumber, char* contents, u32int length, u32int offset) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to read a corrupted page at [%d, %d]", inode.data.nextSector, inode.data.nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
		// log(L_DEBUG, "READING %d bytes from inode %d with %d bytes / %d", length, inodeNumber, inode.data.totalLength, inode.data.usedBytes);
	return _readBlock(&inode.data, contents, length, offset);
}


PRIVATE int _readBlock(DiskPage *page, char *contents, u32int length, u32int offset) {
	// Read FilePage
	DiskPage currPage;
		// log(L_DEBUG,"reding file page: [%d, %d]\n", page->nextSector, page->nextOffset);
	strategy.read(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		log(L_ERROR,"CORRUPTED FILE PAGE! [%d, %d]\n",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	//log(L_DEBUG,"offet: %d / reading File contents: [%d, %d] used: %d\n", offset, page->nextSector, page->nextOffset + offset, currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES);
	int bytesFromContent;
	if (offset < (currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES)) {
		// Start reading from current page
		bytesFromContent = MIN(currPage.totalLength - FILE_BLOCK_OVERHEAD_SIZE_BYTES - offset, length);
		length -= bytesFromContent;
		//	log(L_DEBUG, "reading contents, %d bytes from [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		strategy.read(page->disk, contents, bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		contents += bytesFromContent;
		offset = 0;
	} else
		offset -= (currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES);

	if (bytesFromContent == 0) {
		return 0;
	}
	while(length != 0) {
		int currPageSector = currPage.nextSector;
		int currPageOffset = currPage.nextOffset;
			//log(L_DEBUG, "reading next page [%d, %d]", currPage.nextSector, currPage.nextOffset);
		strategy.read(currPage.disk, &currPage, sizeof(DiskPage), currPage.nextSector, currPage.nextOffset);
		if (offset < currPage.usedBytes - sizeof(DiskPage)) {			// Read from beginning
			bytesFromContent = MIN(currPage.usedBytes - sizeof(DiskPage) - offset, length);
			//	log(L_DEBUG, "reading %d bytes from [%d, %d]", bytesFromContent, currPageSector, currPageOffset + sizeof(DiskPage) + offset);
			strategy.read(page->disk, contents, bytesFromContent, currPageSector, currPageOffset + sizeof(DiskPage) + offset);
			contents += bytesFromContent;
			length -= bytesFromContent;
			offset = 0;
		} else
			offset -= (currPage.usedBytes - sizeof(DiskPage));
	}
	return 0;
}

PRIVATE int _writeBlock(DiskPage *page, char *contents, u32int length, u32int offset) {
	DiskPage currPage;
	//	log(L_DEBUG,"validating file page: [%d, %d]", page->nextSector, page->nextOffset);
	strategy.read(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		log(L_ERROR,"CORRUPTED FILE PAGE! [%d, %d]",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}

	int bytesFromContent;
	//   log(L_DEBUG, "offset: %d / usedBytes: %d, lenght: %d", offset, currPage.usedBytes, length);
	if (offset < (currPage.totalLength - FILE_BLOCK_OVERHEAD_SIZE_BYTES)) {
		// Start reading from current page
		bytesFromContent = MIN(currPage.totalLength - offset - FILE_BLOCK_OVERHEAD_SIZE_BYTES, length);
		length -= bytesFromContent;
			//  log(L_DEBUG, "writing contents, %d bytes to [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		strategy.write(page->disk, contents, bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		contents += bytesFromContent;

		if (FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset + bytesFromContent > currPage.usedBytes) {
			currPage.usedBytes = FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset + bytesFromContent;
			//  log(L_DEBUG, "updating DiskPage, usedBytes = %d + %d + %d = %d", FILE_BLOCK_OVERHEAD_SIZE_BYTES, offset, bytesFromContent, currPage.usedBytes);
			strategy.write(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
		}
		offset = 0;
	} else
		offset -= (currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES);


	int pageSector, pageOffset;
	while(length != 0) {
		pageSector = currPage.nextSector;
		pageOffset = currPage.nextOffset;
		if (!currPage.hasNextPage) {
				log(L_ERROR, "Not enough memory in block to save contents!! expected %d more bytes!", length);
			errno = E_OUT_OF_MEMORY;
			return -1;
		}
		//	log(L_DEBUG, "reading next page [%d, %d] - bytes left: %d", currPage.nextSector, currPage.nextOffset, length);
		strategy.read(currPage.disk, &currPage, sizeof(DiskPage), currPage.nextSector, currPage.nextOffset);
		if (currPage.magic != MAGIC_NUMBER) {
			log(L_ERROR, "CORRUPTED file at [%d, %d]", pageSector, pageOffset);
			errno = E_CORRUPTED_FILE;
			return -1;
		}
			//log(L_DEBUG, "writing (%d) to [%d, %d]", bytesFromContent, pageSector, pageOffset + sizeof(DiskPage));
		if (offset < (currPage.totalLength - sizeof(DiskPage))) {
			bytesFromContent = MIN(currPage.totalLength - sizeof(DiskPage) - offset, length);
			length -= bytesFromContent;
			//    log(L_DEBUG, "writing contents, %d bytes to [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + sizeof(DiskPage) + offset);
			strategy.write(page->disk, contents, bytesFromContent, pageSector, pageOffset + sizeof(DiskPage) + offset);
			contents += bytesFromContent;

			if (sizeof(DiskPage) + offset + bytesFromContent > currPage.usedBytes) {
				currPage.usedBytes = sizeof(DiskPage) + offset + bytesFromContent;
				//	log(L_DEBUG, "updating DiskPage, bytes used is now: %d + %d + %d", sizeof(DiskPage), offset, bytesFromContent);
				strategy.write(page->disk, &currPage, sizeof(DiskPage), pageSector, pageOffset);
			}
			offset = 0;
		} else
			offset -= (currPage.usedBytes - sizeof(DiskPage));
	}
	return 0;
}


PRIVATE int _reserveMemoryBitMap(DiskPage *page, int blocks, u32int initialSector, u32int initialOffset) {
		log(L_DEBUG, "\nReserving %d blocks....", blocks);
	DiskPage currPage;
	int previousSector, previousOffset, currSector, currOffset;
	int disk = ATA0;
	int numberOfblocks = SECTOR_SIZE;		// Use complete sector for the bitmap

	char block[numberOfblocks];
	strategy.read(disk, block, numberOfblocks, FILES_BIT_MAP_SECTOR, 0);

	int reservedBlocks = 0;

	// Iterates for each byte to find a bit turned off (empty slot)
	for(int i = 0; i < numberOfblocks && reservedBlocks < blocks; i++) {
		for (int j = 0; j < 8 && reservedBlocks < blocks; j++) {					// each char has 8 bits!
			currSector = initialSector;
			currOffset = initialOffset + ((i * 8) + j) * DISK_BLOCK_SIZE_BYTES;
			if (BIT(block[i], j) == 0) {											// if block is not used
				block[i] |= (1 << j);
				reservedBlocks++;
				log(L_DEBUG, "Used: [%d, %d] - No: %d / %d", currSector, currOffset, reservedBlocks, blocks);
				currPage.magic = MAGIC_NUMBER;
				currPage.disk = disk;
				currPage.usedBytes = 0;
				currPage.totalLength = DISK_BLOCK_SIZE_BYTES;
				if (reservedBlocks == blocks) {
					currPage.hasNextPage = false;
					//	log(L_DEBUG, "using [%d, %d] - %d", currSector, currOffset, reservedBlocks);
					strategy.write(currPage.disk, &currPage, sizeof(DiskPage), currSector, currOffset);
				}
				if (reservedBlocks > 1) {
					currPage.hasNextPage = true;
					currPage.nextSector = currSector;
					currPage.nextOffset = currOffset;
					//	log(L_DEBUG, "prev - using [%d, %d] - %d", previousSector, previousOffset, reservedBlocks);
					strategy.write(currPage.disk, &currPage, sizeof(DiskPage), previousSector, previousOffset);
				}
				if (reservedBlocks == 1) {		// set up first reserved page info to caller function
					page->disk = ATA0;
					page->nextSector = currSector;
					page->nextOffset = currOffset;
					page->totalLength = blocks * DISK_BLOCK_SIZE_BYTES;
					page->usedBytes = 0;
					page->magic = MAGIC_NUMBER;
					page->hasNextPage = blocks > 1;
					// log(L_DEBUG, "Returning: [%d, %d, %d]. Mem: t:%d / u:%d - next page? %d", page->disk, page->nextSector, page->nextOffset, page->totalLength, page->usedBytes, page->hasNextPage);
				}
				previousSector = currSector;
				previousOffset = currOffset;
			}
		}
	}
	// Update bit map information
	strategy.write(disk, block, numberOfblocks, FILES_BIT_MAP_SECTOR, 0);
	if (reservedBlocks < blocks) {			// Reached end of space available and the recolected space is not enought
		log(L_ERROR, "DISK OUT OF MEMORY!");
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
		return -1;
	}
		// log(L_DEBUG, "finished reserving mem: %d", block[0]);
	return 0;
}

PRIVATE void _freeMemory(DiskPage* page) {
	int numberOfblocks = SECTOR_SIZE;
	char block[numberOfblocks];
	strategy.read(ATA0, block, numberOfblocks, FILES_BIT_MAP_SECTOR, 0);
	// char j, bit i ==> ((i * 8) + j) * DISK_BLOCK_SIZE_BYTES; (sector)
	DiskPage curr;
	curr.disk = page->disk;
	curr.nextOffset = page->nextOffset;
	curr.nextSector = page->nextSector;
	int currPageSector, currPageoffset;
	do {
		currPageSector = curr.nextSector;
		currPageoffset = curr.nextOffset;
		strategy.read(curr.disk, &curr, sizeof(DiskPage), curr.nextSector, curr.nextOffset);
		//	log(L_DEBUG, "Freeing memory, [%d, %d]", currPageSector, currPageoffset);
			currPageoffset /= DISK_BLOCK_SIZE_BYTES;
		int charPos = currPageoffset / 8;
		int bit = currPageoffset % 8;
		block[charPos] &= ~(1 << bit);
		//	log(L_DEBUG, "char: %d, bit: %d", charPos, bit);
	} while (curr.hasNextPage);
	strategy.write(ATA0, block, numberOfblocks, FILES_BIT_MAP_SECTOR, 0);
}

PRIVATE int _extendMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset) {
	DiskPage lastPage;
	u32int lastPageSector = page->nextSector,
	       lastPageOffset = page->nextOffset;
	// log(L_DEBUG, "last: sector: %d, offset: %d", lastPageSector, lastPageOffset);
	strategy.read(page->disk, &lastPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	// Get the end of this memory segment...
	while(lastPage.hasNextPage) {
		lastPageSector = lastPage.nextSector;
		lastPageOffset = lastPage.nextOffset;
		//	log(L_DEBUG, "next page: [%d, %d]", lastPage.nextSector, lastPage.nextOffset);
		strategy.read(lastPage.disk, &lastPage, sizeof(DiskPage), lastPage.nextSector, lastPage.nextOffset);
	}

	int neededBlocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;
	size += neededBlocks * sizeof(FileHeader);
	neededBlocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;

	// Reserve extra memory
	DiskPage cont;
	int reserved = _reserveMemoryBitMap(&cont, neededBlocks, initialSector, initialOffset);
	if (reserved == -1) {		// There was a problem reserving the memory
		return -1;
	}
	page->totalLength = cont.totalLength;
	// Attach extra memory to the end of this segment
	lastPage.hasNextPage = true;
	lastPage.nextSector = cont.nextSector;
	lastPage.nextOffset = cont.nextOffset;
	//	log(L_DEBUG, "saving new DiskPage to: [%d, %d, %d]", lastPage.disk, lastPageSector, lastPageOffset);
	//	log(L_DEBUG, "new DiskPage points to: [%d, %d, %d]", lastPage.disk, lastPage.nextSector, lastPage.nextOffset);
	strategy.write(page->disk, &lastPage, sizeof(DiskPage), lastPageSector, lastPageOffset);
	return neededBlocks;
}


PRIVATE void _setiNode(u32int inodeNumber, iNodeDisk *inode) {
	// log(L_DEBUG, "_setiNode %d node: [%d, %d], size: %d\n", inode, sector, inode * sizeof(FilePage), sizeof(FilePage));
	strategy.write(ATA0, inode, sizeof(iNodeDisk), INODES_INITIAL_SECTOR, inodeNumber * sizeof(iNodeDisk));
}

PRIVATE void _getiNode(u32int inodeNumber, iNodeDisk *inode) {
	strategy.read(ATA0, inode, sizeof(iNodeDisk), INODES_INITIAL_SECTOR, inodeNumber * sizeof(iNodeDisk));
}


void diskManager_getFileName(u32int inode, char *name) {
	FileHeader header;
	errno = 0;
	_getFileheader(inode, &header);
	if (errno == 0) {
		memcpy(name, header.name, MAX_NAME_LENGTH);
	}
}

void diskManager_setFileName(u32int inode, char *name) {
	FileHeader header;
	errno = 0;
	_getFileheader(inode, &header);
	if (errno == 0) {
		memcpy(header.name, name, MAX_NAME_LENGTH);
		_setFileheader(inode, &header);
	}
}

PUBLIC boolean diskManager_setFileMode(u32int inode, int newMode) {
    FileHeader header;
    errno = 0;
    _getFileheader(inode, &header);
    if (errno == 0) {
        header.mask = newMode;
        _setFileheader(inode, &header);
        return true;
    }
    return false;
}

PUBLIC boolean diskManager_setFileUid(u32int inode, int uid) {
    FileHeader header;
    errno = 0;
    _getFileheader(inode, &header);
    if (errno == 0) {
        header.uid = uid;
        _setFileheader(inode, &header);
        return true;
    }
    return false;
}

PUBLIC boolean diskManager_setFileGid(u32int inode, int gid) {
    FileHeader header;
    errno = 0;
    _getFileheader(inode, &header);
    if (errno == 0) {
        header.gid = gid;
        _setFileheader(inode, &header);
        return true;
    }
    return false;
}

void _getFileheader(u32int inodeNumber, FileHeader *header) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to read inode %d with a currupted header at [%d, %d]", inode, inode.data.nextSector, inode.data.nextOffset);
		errno = E_CORRUPTED_FILE;
		return;
	}
		// log(L_DEBUG, "Reading header from: [%d, %d]", inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
	strategy.read(inode.data.disk, header, sizeof(FileHeader), inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
}

void _setFileheader(u32int inodeNumber, FileHeader *header) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "trying to write haeder to non existen file: %d", inodeNumber);
		errno = E_CORRUPTED_FILE;
		return;
	}
	strategy.write(inode.data.disk, header, sizeof(FileHeader), inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
}


u32int diskManager_size(u32int inodeNumber) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	return inode.usedBytes;
}

u32int _availableMem(iNodeDisk* inode) {
	int total = 0;
	if (inode->blocks <= 0) {
		log(L_ERROR, "iNode has no attached memory -> [%d, %d] - blocks: %d\n", inode->data.nextSector, inode->data.nextOffset, inode->blocks);
		return 0;
	}
	total += DISK_BLOCK_SIZE_BYTES - (sizeof(DiskPage) + sizeof(FileHeader));
	total += (inode->blocks - 1) * (DISK_BLOCK_SIZE_BYTES - sizeof(DiskPage));
	return total;
}

