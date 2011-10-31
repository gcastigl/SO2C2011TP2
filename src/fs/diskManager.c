#include <fs/diskManager.h>

#define MIN(x,y)							(((x) < (y)) ? (x) : (y))
#define BIT(n, i)							((n) & (1 << (i)))

#define FILE_CONTENTS_INITAL_SECTOR			2
#define FILE_CONTENTS_INITAL_OFFSET			maxIndes * sizeof(DiskPage)

PRIVATE u32int maxIndes;					// Once setted this value, it should never be changed!!

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
	int numberOfBlocks = SECTOR_SIZE - sizeof(FSHeader), i;
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
	int initialBlocks = 1;
	int reserved = _reserveMemoryBitMap(&newiNode.data, initialBlocks, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	if (reserved == -1) {	// Do not set errno because it should already be set
		log(L_ERROR, "failed to reserve memory for inode %s (%d)", name, inodeNumber);
		return;
	}
	newiNode.blocks = initialBlocks;
	newiNode.usedBytes = 0;
	_setiNode(inodeNumber, &newiNode);
		log(L_DEBUG, "Create inode %s(%d) at [%d, %d], bytes used: %d, max bytes: %d, available %d", name, inodeNumber, 1, \
				inodeNumber * sizeof(iNodeDisk), newiNode.usedBytes, newiNode.blocks * DISK_BLOCK_SIZE_BYTES, _availableMem(&newiNode));
		log(L_DEBUG, "inode points to: [%d, %d]\n", newiNode.data.nextSector, newiNode.data.nextOffset);
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
	diskCache_read(inodeOnDisk.data.disk, &header, sizeof(FileHeader), inodeOnDisk.data.nextSector, inodeOnDisk.data.nextOffset + sizeof(DiskPage));
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


int diskManager_writeContents(u32int inodeNumber, char *contents, u32int length, u32int offset) {
	iNodeDisk inode;
	_getiNode(inodeNumber, &inode);
	//	log(L_DEBUG, "updating contents, %d bytes to inode: %d -> [%d, %d]", length, inodeNumber, inode.data.nextSector, inode.data.nextOffset);
	//	log(L_DEBUG,"Contents[MAX: %d, used: %d], reserved blocks = %d", _availableMem(&inode), inode.usedBytes, inode.blocks);
	if (inode.data.magic != MAGIC_NUMBER) {
		log(L_ERROR, "Trying to write to a corrupted page!");
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	if (offset + length > _availableMem(&inode)) {
		//	log(L_DEBUG, "%d -> Memory is not enough, have: %d, extending memory to %d bytes", inodeNumber, _availableMem(&inode), offset + length);
		int extrablocks = _extendMemory(&inode.data, offset + length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
		inode.blocks += extrablocks;
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
		bytesFromContent = MIN(currPage.totalLength - FILE_BLOCK_OVERHEAD_SIZE_BYTES - offset, length);
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
		// log(L_DEBUG,"validating file page: [%d, %d]", page->nextSector, page->nextOffset);
	diskCache_read(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		log(L_ERROR,"CORRUPTED FILE PAGE! [%d, %d]",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}

	int bytesFromContent;
	if (offset < (currPage.usedBytes - FILE_BLOCK_OVERHEAD_SIZE_BYTES)) {
		// Start reading from current page
		bytesFromContent = MIN(currPage.totalLength - offset - FILE_BLOCK_OVERHEAD_SIZE_BYTES, length);
		length -= bytesFromContent;
		//	log(L_DEBUG, "writing contents, %d bytes to [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		diskCache_write(page->disk, contents, bytesFromContent, page->nextSector, page->nextOffset + FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset);
		contents += bytesFromContent;

		if (FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset + bytesFromContent > currPage.usedBytes) {
			currPage.usedBytes = FILE_BLOCK_OVERHEAD_SIZE_BYTES + offset + bytesFromContent;
		//	log(L_DEBUG, "updating DiskPage, usedBytes = %d + %d + %d", FILE_BLOCK_OVERHEAD_SIZE_BYTES, offset, bytesFromContent);
			diskCache_write(page->disk, &currPage, sizeof(DiskPage), page->nextSector, page->nextOffset);
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
		diskCache_read(currPage.disk, &currPage, sizeof(DiskPage), currPage.nextSector, currPage.nextOffset);
		if (currPage.magic != MAGIC_NUMBER) {
			log(L_ERROR, "CORRUPTED file at [%d, %d]", pageSector, pageOffset);
			errno = E_CORRUPTED_FILE;
			return -1;
		}
			//log(L_DEBUG, "writing (%d) to [%d, %d]", bytesFromContent, pageSector, pageOffset + sizeof(DiskPage));
		if (offset < (currPage.usedBytes - sizeof(DiskPage))) {
			bytesFromContent = MIN(currPage.totalLength - sizeof(DiskPage), length);
			length -= bytesFromContent;
			//	log(L_DEBUG, "writing contents, %d bytes to [%d, %d]", bytesFromContent, page->nextSector, page->nextOffset + sizeof(DiskPage) + offset);
			diskCache_write(page->disk, contents, bytesFromContent, pageSector, pageOffset + sizeof(DiskPage) + offset);
			contents += bytesFromContent;

			if (sizeof(DiskPage) + offset + bytesFromContent > currPage.usedBytes) {
				currPage.usedBytes = sizeof(DiskPage) + offset + bytesFromContent;
				//	log(L_DEBUG, "updating DiskPage, bytes used is now: %d + %d + %d", sizeof(DiskPage), offset, bytesFromContent);
				diskCache_write(page->disk, &currPage, sizeof(DiskPage), pageSector, pageOffset);
			}
			offset = 0;
		} else
			offset -= (currPage.usedBytes - sizeof(DiskPage));
	}
	return 0;
}

PRIVATE int _reserveMemoryBitMap(DiskPage *page, int blocks, u32int initialSector, u32int initialOffset) {
		log(L_DEBUG, "\nReserving %d blocks....", blocks);
	_cli();
	DiskPage currPage;
	int previousSector, previousOffset, currSector, currOffset;
	int disk = ATA0;
	int numberOfblocks = SECTOR_SIZE - sizeof(FSHeader);	// Use all remaining bits until the end of the first sector

	char block[numberOfblocks];
	diskCache_read(disk, block, numberOfblocks, 0, sizeof(FSHeader));

	int reservedBlocks = 0;

	// Iterates for each byte to find a bit turned off (empty slot)
	for(int i = 0; i < numberOfblocks && reservedBlocks < blocks; i++) {
		for (int j = 0; j < 8 && reservedBlocks < blocks; j++) {					// each char has 8 bits!
			currSector = initialSector;
			currOffset = initialOffset + ((i * 8) + j) * DISK_BLOCK_SIZE_BYTES;
				log(L_DEBUG, "[%d, %d] -> %s", currSector, currOffset, (BIT(block[i], j) == 0) ? "Free" : "Used");
			if (BIT(block[i], j) == 0) {											// if block is not used
				block[i] |= (1 << j);
				reservedBlocks++;
				log(L_DEBUG, " => Used %d", reservedBlocks);
				currPage.magic = MAGIC_NUMBER;
				currPage.disk = disk;
				currPage.usedBytes = 0;
				currPage.totalLength = DISK_BLOCK_SIZE_BYTES;
				if (reservedBlocks == blocks) {
					currPage.hasNextPage = false;
					//	log(L_DEBUG, "using [%d, %d] - %d", currSector, currOffset, reservedBlocks);
					diskCache_write(currPage.disk, &currPage, sizeof(DiskPage), currSector, currOffset);
				}
				if (reservedBlocks > 1) {
					currPage.hasNextPage = true;
					currPage.nextSector = currSector;
					currPage.nextOffset = currOffset;
					//	log(L_DEBUG, "prev - using [%d, %d] - %d", previousSector, previousOffset, reservedBlocks);
					diskCache_write(currPage.disk, &currPage, sizeof(DiskPage), previousSector, previousOffset);
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
	diskCache_write(disk, block, numberOfblocks, 0, sizeof(FSHeader));
	if (reservedBlocks < blocks) {			// Reached end of space available and the recolected space is not enought
		log(L_ERROR, "DISK OUT OF MEMORY!");
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
		_sti();
		return -1;
	}
		// log(L_DEBUG, "finished reserving mem: %d", block[0]);
	_sti();
	return 0;
}

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
}

PRIVATE int _extendMemory(DiskPage *page, int size, u32int initialSector, u32int initialOffset) {
	u32int disk = ATA0;
	DiskPage lastPage;
	u32int lastPageSector = page->nextSector,
			lastPageOffset = page->nextOffset;
		// log(L_DEBUG, "last: sector: %d, offset: %d", lastPageSector, lastPageOffset);
	memcpy(&lastPage, page, sizeof(DiskPage));
	// Get the end of this memory segment...
	while(lastPage.hasNextPage) {
		lastPageSector = lastPage.nextSector;
		lastPageOffset = lastPage.nextOffset;
			// log(L_DEBUG, "reading from [%d, %d]", lastPage.nextSector, lastPage.nextOffset);
		diskCache_read(disk, &lastPage, sizeof(DiskPage), lastPage.nextSector, lastPage.nextOffset);
	}

	int neededBlocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;
	size += neededBlocks * sizeof(FileHeader);
	neededBlocks = (size / (DISK_BLOCK_SIZE_BYTES + 1)) + 1;

	// Reserve extra memory
	DiskPage cont;
	int reserved = _reserveMemoryBitMap(&cont, neededBlocks, initialSector, initialOffset);
	if (reserved == -1) {		// There was a problem reserving the memory
		return 0;
	}
	page->totalLength = cont.totalLength;
	// Attach extra memory to the end of this segment
	lastPage.hasNextPage = true;
	lastPage.nextSector = cont.nextSector;
	lastPage.nextOffset = cont.nextOffset;
	//	log(L_DEBUG, "saving new DiskPage to: [%d, %d, %d]", disk, lastPageSector, lastPageOffset);
	//	log(L_DEBUG, "new DiskPage points to: [%d, %d, %d]", disk, lastPage.nextSector, lastPage.nextOffset);
	diskCache_write(disk, &lastPage, sizeof(DiskPage), lastPageSector, lastPageOffset);
	return neededBlocks;
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

void diskManager_setFileMode(u32int inode, int mode) {
    mode &= (S_IRWXU | S_IRWXG | S_IRWXO);
    FileHeader header;
    errno = 0;
    _getFileheader(inode, &header);
    if (errno == 0) {
        int new = (header.mask & FS_TYPE) | mode;
        log(L_DEBUG, "changing inode %d to %x mode [old: %x]",
                inode,
                new,
                header.mask
        );
        header.mask = new;
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
