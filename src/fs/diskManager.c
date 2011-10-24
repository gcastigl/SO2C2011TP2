#include <fs/diskManager.h>

#define MIN(x,y)							(((x) < (y)) ? (x) : (y))
#define BIT(n, i)							((n) & (1 << (i)))

#define FILE_CONTENTS_INITAL_SECTOR			2
#define FILE_CONTENTS_INITAL_OFFSET			maxIndes * sizeof(DiskPage)

PRIVATE u32int maxIndes;					// Once setted this value, it should never be changed!!

PRIVATE void _getiNode(u32int inodeNumber, iNodeDisk *inode);
PRIVATE void _setiNode(u32int inodeNumber, iNodeDisk *inode);

PRIVATE int _reserveMemoryBitMap(DiskPage *page, int size, u32int initialSector, u32int initialOffset);
//PRIVATE int _reserveMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset);
PRIVATE int _extendMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset);
PRIVATE void _freeMemory(DiskPage *page);

PRIVATE int _readBlock(DiskPage *page, char *contents, u32int length, u32int offset);
PRIVATE int _writeBlock(DiskPage *page, char *contents, u32int length, u32int offsett);

void _setFileheader(u32int inodeNumber, FileHeader *header);
void _getFileheader(u32int inodeNumber, FileHeader *header);

void diskManager_init(u32int maxNodes) {
	log(L_DEBUG, "Using block size: %d", DISK_BLOCK_SIZE_BYTES);
	log(L_DEBUG, "DiskPage size: %d", sizeof(DiskPage));
	log(L_DEBUG, "FileHeader size: %d", sizeof(FileHeader));
}

boolean diskManager_validateHeader() {
	FSHeader header;
	diskCache_read(ATA0, &header, sizeof(FSHeader), 0, 0);
	if (header.magic == MAGIC_NUMBER) {
		maxIndes = header.maxNodes;
	}
		log(L_DEBUG, "Header for the OS is %svalid", header.magic == MAGIC_NUMBER ? "" : "NOT ");
	return header.magic == MAGIC_NUMBER;
}

void diskManager_writeHeader(u32int maxNodes) {
	FSHeader header;
	header.magic = MAGIC_NUMBER;
	header.totalNodes = 0;
	header.maxNodes = maxNodes;
	maxIndes = maxNodes;
		log(L_DEBUG, "Writing OS header");
	diskCache_write(ATA0, &header, sizeof(FSHeader), 0, 0);
	// Mark all sector as free sectors
	int numberOfBlocks = 512 + sizeof(FSHeader), i;
	char block[numberOfBlocks];
	for (i = 0; i < numberOfBlocks; i++) block[i] = 0;
	diskCache_write(ATA0, block, numberOfBlocks, 0, sizeof(FSHeader));
}

int diskManager_nextInode() {
	FSHeader header;
	diskCache_read(ATA0, &header, sizeof(FSHeader), 0, 0);
	int nextiNode = header.totalNodes++;
		log(L_DEBUG, "next inodeNumber to save in disk => %d", nextiNode);
	diskCache_write(ATA0, &header, sizeof(FSHeader), 0, 0);			// Update totalNodes value
	return nextiNode;
}


void diskManager_createInode(iNode* inode, u32int inodeNumber, char* name) {
	iNodeDisk newiNode;
	int reservedBlocks = _reserveMemoryBitMap(&newiNode.data, FILE_INITIAL_SIZE_BYTES, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	newiNode.totalReservedMem = newiNode.data.totalLength;
	newiNode.usedMem  = reservedBlocks * sizeof(DiskPage) + sizeof(FileHeader);
	newiNode.contentMaxBytes = newiNode.totalReservedMem - newiNode.usedMem;
	newiNode.contentUsedBytes = 0;
	_setiNode(inodeNumber, &newiNode);
		log(L_DEBUG, "Create inode %d, %s at [%d, %d], Contents[MAX: %d, used: %d], Total[MAX: %d, used: %d]\n", inodeNumber, name, 1, inodeNumber * sizeof(iNodeDisk), newiNode.contentMaxBytes, newiNode.contentUsedBytes, newiNode.totalReservedMem, newiNode.usedMem);
	FileHeader header;
	header.magic = MAGIC_NUMBER;
	strcpy(header.name, name);
	header.gid = inode->gid;
	header.uid  =inode->uid;
	header.flags = inode->flags;
	header.impl = inode->impl;
	header.mask = inode->mask;
	diskCache_write(newiNode.data.disk, &header, sizeof(FileHeader), newiNode.data.nextSector, newiNode.data.nextOffset + sizeof(DiskPage));
		// log(L_DEBUG, "written inode header to: [%d, %d + %d]", newiNode.data.nextSector, newiNode.data.nextOffset, sizeof(DiskPage));
}

void diskManager_readInode(iNode *inode, u32int inodeNumber, char* name) {
	iNodeDisk inodeOnDisk;
	_getiNode(inodeNumber, &inodeOnDisk);
		// log(L_DEBUG, "reading from inode: %d -> [%d, %d]", inodeNumber, inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset);
	if (inodeOnDisk.data.magic != MAGIC_NUMBER) {
			log(L_ERROR, "Can't read iNode %d - Corrupted file!", inodeNumber);
		errno = E_CORRUPTED_FILE;
		return;
	}
	FileHeader header;
	diskCache_read(inodeOnDisk.data.disk, &header, sizeof(FileHeader), inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset + sizeof(DiskPage));
	if (header.magic != MAGIC_NUMBER) {
			log(L_ERROR, "Can't read file header (%d) - Corrupted->[%d, %d]", inodeNumber, inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset);
		errno = E_CORRUPTED_FILE;
		return;
	}
	strcpy(header.name, name);
	inode->gid = header.gid;
	inode->uid = header.uid;
	inode->flags = header.flags;
	inode->impl = header.impl;
	inode->mask = header.mask;

	inode->inodeId = inodeNumber;
	inode->length = inodeOnDisk.contentUsedBytes;
	inode->sector = inodeOnDisk.data.nextSector;
	inode->offset = inodeOnDisk.data.nextOffset;
}


int diskManager_writeContents(u32int inodeNumber, char *contents, u32int length, u32int offset) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
		log(L_DEBUG, "updating contents, %d bytes to inode: %d -> [%d, %d]", length, inodeNumber, inode.data.nextSector, inode.data.nextOffset);
		// log(L_DEBUG,"Contents[MAX: %d, used: %d], Total[MAX: %d, used: %d]", inode.contentMaxBytes, inode.contentUsedBytes, inode.totalReservedMem, inode.usedMem);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to write to a corrupted page!");
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	inode.usedMem = inode.usedMem - inode.contentUsedBytes + length;
	if (length > inode.totalReservedMem) {
			log(L_DEBUG, "Size is not enough, have: %d, used %d... and need to save %d total bytes - extending memory", inode.totalReservedMem, inode.usedMem, length);
		int extrablocks = _extendMemory(&inode.data, length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
		inode.usedMem += extrablocks * sizeof(DiskPage);
		inode.totalReservedMem += inode.data.totalLength;
	}
	int status = _writeBlock(&inode.data, contents, length, offset);
	if (status == -1) {
		log(L_ERROR, "\n_writeBlock has failed writing contents...\n");
		return -1;
	}
	// FIXME: this is only true when over writting all existing contents...
	inode.contentUsedBytes = length;
	_setiNode(inodeNumber, &inode);
	return 0;
}

int diskManager_readContents(u32int inodeNumber, char* contents, u32int length, u32int offset) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to a corrupted page at [%d, %d]", inode.data.nextSector, inode.data.nextOffset);
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
	diskCache_read(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		log(L_ERROR,"CORRUPTED FILE PAGE! [%d, %d]\n",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	// log(L_DEBUG,"reading File contents: [%d, %d] %d\n", page->nextSector, page->nextOffset + offset, page->usedBytes);
	int bytesFromContent;
	if (offset < (currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES)) {
		// Start reading from current page
		bytesFromContent = MIN(currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES - offset, length);
		length -= bytesFromContent;
			// log(L_DEBUG, "reading contents, %d bytes from [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		diskCache_read(page->disk, contents, bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
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
		diskCache_read(currPage.disk, &currPage, sizeof(DiskPage), currPage.nextSector, currPage.nextOffset);
		if (offset < currPage.usedBytes - sizeof(DiskPage)) {			// Read from beginning
			bytesFromContent = MIN(currPage.usedBytes - sizeof(DiskPage) - offset, length);
				// log(L_DEBUG, "reading %d bytes from [%d, %d]", bytesFromContent, currPageSector, currPageOffset + sizeof(DiskPage) + offset);
			diskCache_read(page->disk, contents, bytesFromContent, currPageSector, currPageOffset + sizeof(DiskPage) + offset);
			contents += bytesFromContent;
			length -= bytesFromContent;
			offset = 0;
		} else
			offset -= (currPage.usedBytes - sizeof(DiskPage));
	}
	return 0;
}

// FIXME: This function always writes from the beginning all the way to the end!
PRIVATE int _writeBlock(DiskPage *page, char *contents, u32int length, u32int offset) {
	DiskPage currPage;
	int total = length;
		// log(L_DEBUG,"validating file page: [%d, %d]", page->nextSector, page->nextOffset);
	diskCache_read(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		log(L_ERROR,"CORRUPTED FILE PAGE! [%d, %d]",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	if (offset != 0) {
		log(L_ERROR, "_writeBlock does not support writing with a given offset yet - off: %d", offset);
		errno = -404;
		return -1;
	}
	int bytesFromContent = MIN(currPage.totalLength - FILE_BLOCK_OVERHEAD_SIZE_BYTES, length);
		// log(L_DEBUG,"TOTAL %d, OVERHEAD = %d / Available: %d,  length = %d", currPage.totalLength, FILE_BLOCK_OVERHEAD_SIZE_BYTES, currPage.totalLength - FILE_BLOCK_OVERHEAD_SIZE_BYTES, length);
		// log(L_DEBUG,"writing contents to: [%d, %d]len:%d", page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES, bytesFromContent);
	diskCache_write(page->disk, contents, bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES);
	length -= bytesFromContent;
	contents += bytesFromContent;

	currPage.usedBytes = FILE_BLOCK_OVERHEAD_SIZE_BYTES + bytesFromContent;
		// log(L_DEBUG, "Updating DiskPage... [%d, %d], used %d, total %d", page->nextSector, page->nextOffset, currPage.usedBytes, currPage.totalLength);
	diskCache_write(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);

	int pageSector, pageOffset;
	while(length != 0) {
		pageSector = currPage.nextSector;
		pageOffset = currPage.nextOffset;
		if (!currPage.hasNextPage) {
				log(L_ERROR, "Not enough memory in block to save contents!! expected %d more bytes!", length);
			errno = E_OUT_OF_MEMORY;
			return -1;
		}
			//log(L_DEBUG, "Next page [%d, %d]- bytes left: %d", currPage.nextSector, currPage.nextOffset, length);
		diskCache_read(currPage.disk, &currPage, sizeof(DiskPage), currPage.nextSector, currPage.nextOffset);
		if (currPage.magic == MAGIC_NUMBER) {
			bytesFromContent = MIN(currPage.totalLength - sizeof(DiskPage), length);
				//log(L_DEBUG, "writing (%d) to [%d, %d]", bytesFromContent, pageSector, pageOffset + sizeof(DiskPage));
			diskCache_write(currPage.disk, contents, bytesFromContent, pageSector, pageOffset + sizeof(DiskPage));
			length -= bytesFromContent;
			contents += bytesFromContent;
			currPage.usedBytes = bytesFromContent + sizeof(DiskPage);
			diskCache_write(page->disk, &currPage, sizeof(DiskPage), pageSector, pageOffset);
		} else {
			log(L_ERROR, "CORRUPTED file at [%d, %d]", page->nextSector, page->nextOffset);
			errno = E_CORRUPTED_FILE;
			return -1;
		}
	}
	page->usedBytes = total;
		// log(L_DEBUG, "Total used bytes after writing: %d", page->usedBytes);
	return 0;
}

PRIVATE int _reserveMemoryBitMap(DiskPage *page, int size, u32int initialSector, u32int initialOffset) {
	_cli();
	DiskPage currPage;
	int previousSector, previousOffset, currSector, currOffset;
	int disk = ATA0;
	int numberOfblocks = 512 - sizeof(FSHeader);

	char block[numberOfblocks];
	diskCache_read(disk, block, numberOfblocks, 0, sizeof(FSHeader));
	int neededBlocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;
	int reservedBlocks = 0;
		log(L_DEBUG, "\nReserving %d bytes => total blocks %d\n", size, neededBlocks);

	// Iterates for each byte to find a bit turned off (empty slot)
	for(int i = 0; i < numberOfblocks && reservedBlocks < neededBlocks; i++) {
		for (int j = 0; j < 8 && reservedBlocks < neededBlocks; j++) {					// each char has 8 bits!
			currSector = initialSector;
			currOffset = initialOffset + ((i * 8) + j) * DISK_BLOCK_SIZE_BYTES;
				log(L_DEBUG, "[%d, %d] -> %s", currSector, currOffset, (BIT(block[i], j) == 0) ? "Free" : "Used");
			if (BIT(block[i], j) == 0) {											// if block is not used
				block[i] |= (1 << j);
				reservedBlocks++;
				currPage.magic = MAGIC_NUMBER;
				currPage.disk = disk;
				currPage.usedBytes = 0;
				currPage.totalLength = DISK_BLOCK_SIZE_BYTES;
				if (reservedBlocks == neededBlocks) {
					currPage.hasNextPage = false;
					log(L_DEBUG, "using [%d, %d] - %d", currSector, currOffset, currPage);
					diskCache_write(currPage.disk, &currPage, sizeof(DiskPage), currSector, currOffset);
				}
				if (reservedBlocks > 1) {
					currPage.hasNextPage = true;
					currPage.nextSector = currSector;
					currPage.nextOffset = currOffset;
					log(L_DEBUG, "prev - using [%d, %d] - %d", previousSector, previousOffset, currPage);
					diskCache_write(currPage.disk, &currPage, sizeof(DiskPage), previousSector, previousOffset);
				}
				if (reservedBlocks == 1) {		// set up first reserved page info to caller function
					page->disk = ATA0;
					page->nextSector = currSector;
					page->nextOffset = currOffset;
					page->totalLength = neededBlocks * DISK_BLOCK_SIZE_BYTES;
					page->usedBytes = 0;
					page->magic = MAGIC_NUMBER;
					page->hasNextPage = neededBlocks > 1;
					log(L_DEBUG, "Returning: [%d, %d, %d]. Mem: t:%d / u:%d - next page? %d", page->disk, page->nextSector, page->nextOffset, page->totalLength, page->usedBytes, page->hasNextPage);
				}
				previousSector = currSector;
				previousOffset = currOffset;
			}
		}
	}
	// Update bit map information
	diskCache_write(disk, block, numberOfblocks, 0, sizeof(FSHeader));
	if (reservedBlocks < neededBlocks) {			// Reached end of space available and the recolected space is not enought
		log(L_ERROR, "DISK OUT OF MEMORY!");
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
	}
		// log(L_DEBUG, "finished reserving mem: %d", block[0]);
	_sti();
	return 0;
}
/*
PRIVATE int _reserveMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset) {
	_cli();
	u32int disk = ATA0;
	DiskPage currPage;
	int sector = initialSector;
	int offset = initialOffset;
	int reserved = 0;
	int previousSector, previousOffset;

	u32int maxOffset = 16384;	// FIXME: implement driveCapacity() in the ata_driver

	int blocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;
		log(L_DEBUG, "\nReserving %d bytes => total blocks %d\n", size, blocks);
	while (reserved < blocks && offset < maxOffset) {
		diskCache_read(ATA0, &currPage, sizeof(DiskPage), sector, offset);
		log(L_DEBUG, "[%d, %d] -> %s", sector, offset, currPage.magic == MAGIC_NUMBER ? "Used" : "free");
		if (currPage.magic != MAGIC_NUMBER) {										// The block is empty... can be used
			currPage.magic = MAGIC_NUMBER;
			currPage.hasNextPage = false;
			currPage.usedBytes = 0;
			currPage.totalLength = DISK_BLOCK_SIZE_BYTES;
			currPage.disk = disk;
			log(L_DEBUG, "using [%d, %d] - %d", sector, offset, currPage);
			diskCache_write(disk, &currPage, sizeof(DiskPage), sector, offset);			// write header to disk
			if (reserved > 0) {														// Update previous header to point to this one
					// log(L_DEBUG, "reading prevoius header [%d, %d]", previousSector, previousOffset);
				diskCache_read(disk, &currPage, sizeof(DiskPage), previousSector, previousOffset);
				currPage.nextSector = sector;
				currPage.nextOffset = offset;
				currPage.hasNextPage = true;
				currPage.disk = disk;
				diskCache_write(disk, &currPage, sizeof(DiskPage), previousSector, previousOffset);
			} else {																// Return first disk position to return
				page->disk = disk;
				page->nextSector = sector;
				page->nextOffset = offset;
				page->usedBytes = 0;
				page->totalLength = DISK_BLOCK_SIZE_BYTES * blocks;
				page->magic = MAGIC_NUMBER;
			}
			previousSector = sector;
			previousOffset = offset;
			reserved++;
		}
		offset += DISK_BLOCK_SIZE_BYTES;
	}
	if (offset >= maxOffset) {			// Reached end of space available and the recolected spaace is not enought
		log(L_ERROR, "DISK OUT OF MEMORY!");
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
	}
	_sti();
		// log(L_DEBUG, "finished reserving memory\n");
	return blocks;
}*/

PRIVATE void _freeMemory(DiskPage* page) {
	int numberOfblocks = 512 - sizeof(FSHeader);
	char block[numberOfblocks];
	diskCache_read(page->disk, block, numberOfblocks, 0, sizeof(FSHeader));
	// char j, bit i ==> ((i * 8) + j) * DISK_BLOCK_SIZE_BYTES; (sector)
	do {
		int sector = page->nextSector;
		int offset = page->nextOffset;
		if (sector % DISK_BLOCK_SIZE_BYTES != 0) {
			log(L_ERROR, "Trying to free an invalid page at [%d, %d]", sector, offset);
		}
		offset /= DISK_BLOCK_SIZE_BYTES;
		int charPos = offset / 8;
		int bit = offset % 8;
		block[charPos] |= 1 << bit;
		log(L_DEBUG, "Freeing memory, [%d, %d], char: %d, bit: %d",charPos, bit);
	} while (page->hasNextPage);
	diskCache_read(page->disk, block, numberOfblocks, 0, sizeof(FSHeader));
	/*DiskPage currPage;
	u32int sector = page->nextSector;
	u32int offset = page->nextOffset;
	do {
		diskCache_read(page->disk, &currPage, sizeof(DiskPage), sector, offset);
		sector = currPage.nextSector;
		offset = currPage.nextOffset;
		currPage.magic = 0;
		diskCache_write(page->disk, &currPage, sizeof(DiskPage), sector, offset);
	} while(currPage.hasNextPage);*/
}

PRIVATE int _extendMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset) {
	u32int disk = ATA0;
	DiskPage lastPage;
	u32int lastPageSector = page->nextSector,
			lastPageOffset = page->nextOffset;
		log(L_DEBUG, "last: sector: %d, offset: %d", lastPageSector, lastPageOffset);
	memcpy(&lastPage, page, sizeof(DiskPage));
	// Get the end of this memory segment...
	while(lastPage.hasNextPage) {
		lastPageSector = lastPage.nextSector;
		lastPageOffset = lastPage.nextOffset;
			log(L_DEBUG, "reading from [%d, %d]", lastPage.nextSector, lastPage.nextOffset);
		diskCache_read(disk, &lastPage, sizeof(DiskPage), lastPage.nextSector, lastPage.nextOffset);
	}

	// Reserve extra memory
	DiskPage cont;
	int extraBlocks = _reserveMemoryBitMap(&cont, size, initialSector, initialOffset);
	page->totalLength = cont.totalLength;
	// Attach extra memory to the end of this segment
	lastPage.hasNextPage = true;
	lastPage.nextSector = cont.nextSector;
	lastPage.nextOffset = cont.nextOffset;
		// log(L_DEBUG, "saving to: [%d, %d, %d]", disk, lastPageSector, lastPageOffset);
	diskCache_write(disk, &lastPage, sizeof(DiskPage), lastPageSector, lastPageOffset);
	return extraBlocks;
}


PRIVATE void _setiNode(u32int inodeNumber, iNodeDisk *inode) {
	// log(L_DEBUG, "_setiNode %d node: [%d, %d], size: %d\n", inode, sector, inode * sizeof(FilePage), sizeof(FilePage));
	diskCache_write(ATA0, inode, sizeof(iNodeDisk), 1, inodeNumber * sizeof(iNodeDisk));
}

PRIVATE void _getiNode(u32int inodeNumber, iNodeDisk *inode) {
	diskCache_read(ATA0, inode, sizeof(iNodeDisk), 1, inodeNumber * sizeof(iNodeDisk));
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


void _getFileheader(u32int inodeNumber, FileHeader *header) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to read inode %d with a currupted header at [%d, %d]", inode, inode.data.nextSector, inode.data.nextOffset);
		errno = E_CORRUPTED_FILE;
		return;
	}
		// log(L_DEBUG, "Reading header from: [%d, %d]", inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
	diskCache_read(inode.data.disk, header, sizeof(FileHeader), inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
}

void _setFileheader(u32int inodeNumber, FileHeader *header) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	if (inode.data.magic != MAGIC_NUMBER) {
		errno = E_CORRUPTED_FILE;
		return;
	}
	diskCache_write(inode.data.disk, header, sizeof(FileHeader), inode.data.nextSector, inode.data.nextOffset + sizeof(DiskPage));
}


u32int diskManager_size(u32int inodeNumber) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	return inode.contentUsedBytes;
}
