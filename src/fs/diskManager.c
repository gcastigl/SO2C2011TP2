#include <fs/diskManager.h>

#define MIN(x,y)							((x) < (y)) ? (x) : (y)

#define FILE_CONTENTS_INITAL_SECTOR			2
#define FILE_CONTENTS_INITAL_OFFSET			maxIndes * sizeof(FilePage)

PRIVATE u32int maxIndes;					// Once setted this value, it should never be changed!!

PRIVATE u32int currDisk;


PRIVATE void _getiNode(int inode, FilePage *contents);
PRIVATE void _setiNode(u32int inode, FilePage *contents);

PRIVATE void _reserveMemory(FilePage *page, int size, u32int initialSector, u32int initialOffset);
PRIVATE void _freeMemory(FilePage *page);

PRIVATE int _readBlock(FilePage *page, FileHeader *header, char *contents, u32int lenght);
PRIVATE int _writeBlock(FilePage *page, FileHeader *header, char* contents, u32int length);

void _setFileheader(int inode, FileHeader *header);
void _getFileheader(int inode, FileHeader *header);

void diskManager_init(u32int maxNodes) {
	currDisk = ATA0;
}

boolean diskManager_validateHeader() {
	FSHeader header;
	ata_read(currDisk, &header, sizeof(FSHeader), 0, 0);
	if (header.magic == MAGIC_NUMBER) {
		maxIndes = header.maxNodes;
	}
	return header.magic == MAGIC_NUMBER;
}

void diskManager_writeHeader(u32int maxNodes) {
	FSHeader header;
	header.magic = MAGIC_NUMBER;
	header.totalNodes = 0;
	header.maxNodes = maxNodes;
	maxIndes = maxNodes;
	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);
}

int diskManager_nextInode() {
	FSHeader header;
	ata_read(currDisk, &header, sizeof(FSHeader), 0, 0);
	int nextiNode = header.totalNodes++;
	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);			// Update totalNodes value
	return nextiNode;
}

void diskManager_writeContents(iNode *inode, char* contents, u32int length) {
	FilePage page;
	_getiNode(inode->inodeId, &page);
	if (page.magic != MAGIC_NUMBER) {
		printf("\nPage for inode %d does not exit, creating a new one...\n", inode->inodeId);
		_reserveMemory(&page, length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
		page.magic = MAGIC_NUMBER;
	}
	page.usedBytes = length;
	_setiNode(inode->inodeId, &page);
	// Save all inode info to fileheader
	FileHeader header;
	header.magic = MAGIC_NUMBER;
	header.flags = inode->flags;
	header.gid = inode->gid;
	header.uid = inode->uid;
	header.impl = inode->impl;
	header.mask = inode->mask;
	diskManager_getFileName(inode->inodeId, header.name);
	inode->sector = page.nextSector;
	inode->offset = page.nextOffset;
	inode->length = length;
	_writeBlock(&page, &header, contents, length);
}

void diskManager_readiNode(iNode *inode, int inodeNumber) {
	FilePage page;
	_getiNode(inodeNumber, &page);
	if (page.magic != MAGIC_NUMBER) {
		printf("CORRUPTED FILE![%d, %d]\n", page.nextSector, page.nextOffset);
		errno = E_CORRUPTED_FILE;
		return;
	}
	FileHeader header;
	ata_read(currDisk, &header, sizeof(FileHeader), page.nextSector, page.nextOffset + sizeof(FilePage));
	inode->flags = header.flags;
	inode->gid = header.gid;
	inode->uid = header.uid;
	inode->impl = header.impl;
	inode->mask = header.mask;
	inode->inodeId = inodeNumber;
	inode->length = page.usedBytes;
	inode->sector = page.nextSector;
	inode->offset = page.nextOffset;
}

char *diskManager_readContents(u32int inodeNumber, int *length) {
	FilePage page;
	_getiNode(inodeNumber, &page);
	char* contents = kmalloc(page.usedBytes);
	*length = page.usedBytes;
	if (page.usedBytes == 0) {
		return NULL;
	}
	FileHeader header;
	_readBlock(&page, &header, contents, page.usedBytes);
	return contents;
}

PRIVATE void _freeMemory(FilePage* page) {
	FilePage currPage;
	u32int disk = currDisk;
	u32int sector = page->nextSector;
	u32int offset = page->nextOffset;
	do {
		ata_read(disk, &currPage, sizeof(FilePage), sector, offset);
		sector = currPage.nextSector;
		offset = currPage.nextOffset;
		currPage.magic = 0;
		ata_write(disk, &currPage, sizeof(FilePage), sector, offset);
	} while(currPage.hasNextPage);
}

PRIVATE int _readBlock(FilePage *page, FileHeader *header, char *contents, u32int lenght) {
	u32int disk = currDisk;
	int offset = 0;
	// Read FilePage
	FilePage currPage;
	//	printf("reding header: [%d, %d]\n", page->nextSector, page->nextOffset);
	ata_read(disk, &currPage, sizeof(FilePage), page->nextSector, page->nextOffset);
	offset += sizeof(FilePage);
	if (currPage.magic != MAGIC_NUMBER) {
		printf("CORRUPTED FILE! [%d, %d]\n",  page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	// Read FileHeader
	//	printf("reaading File header: [%d, %d]\n", page->nextSector, page->nextOffset + offset);
	ata_read(disk, header, sizeof(FileHeader), page->nextSector, page->nextOffset + offset);
	offset += sizeof(FileHeader);
	if (header->magic != MAGIC_NUMBER) {
		printf("CORRUPTED FILE! [%d, %d]\n", page->nextSector, offset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	//	printf("reading File contents: [%d, %d] %d\n", page->nextSector, page->nextOffset + offset, page->usedBytes);
	ata_read(disk, contents, page->usedBytes, page->nextSector, page->nextOffset + offset);
	contents += page->usedBytes;
	/*while(currPage.hasNextPage) {
		// For each FilePage, read it's contents
		ata_read(disk, &currPage, sizeof(FilePage), currPage.nextSector, currPage.nextOffset);
		ata_read(disk, contents, currPage.usedBytes, sector, sizeof(FilePage) + currPage.nextOffset);
		sector = currPage.nextSector;
		offset = currPage.nextOffset;
		contents += currPage.usedBytes;
	}*/
	return 0;
}

PRIVATE int _writeBlock(FilePage *page, FileHeader *header, char* contents, u32int length) {
	int disk = currDisk;
	int offset;
	// Save FilePage
	FilePage currPage;
	ata_read(disk, &currPage, sizeof(FilePage), page->nextSector, page->nextOffset);
	if (currPage.magic != MAGIC_NUMBER) {
		printf("THE FILE IS CORRUPTED! [%d, %d]\n", page->nextSector, page->nextOffset);
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	offset = 0;
	// Update FilePage values
	currPage.usedBytes = MIN(currPage.totalLength - sizeof(FilePage) - sizeof(FileHeader), length);
	if (currPage.usedBytes == length) {
		currPage.hasNextPage = false;
	}
	//	printf("writing filepage [%d,%d]s: %d\n", page->nextSector, page->nextOffset, sizeof(FilePage));
	ata_write(disk, &currPage, sizeof(FilePage), page->nextSector, page->nextOffset);
	offset += sizeof(FilePage);

	// Save FileHeader
	//	printf("writing header [%d,%d]s: %d\n", page->nextSector, page->nextOffset + offset, sizeof(FileHeader));
	ata_write(disk, header, sizeof(FileHeader), page->nextSector, page->nextOffset + offset);
	offset += sizeof(FileHeader);

	// Save contents
	//	printf("writing contents [%d, %d]s: %d / %s\n", page->nextSector, page->nextOffset + offset, toWrite, contents);
	ata_write(disk, contents, currPage.usedBytes, page->nextSector, page->nextOffset + offset);
	contents += currPage.usedBytes;
	length -= currPage.usedBytes;
	while(length != 0) {
		ata_read(disk, &currPage, sizeof(FilePage), currPage.nextSector, currPage.nextOffset);
		if (currPage.magic == MAGIC_NUMBER) {
			currPage.usedBytes = MIN(currPage.totalLength - sizeof(FilePage) - sizeof(FileHeader), length);
			if (currPage.usedBytes == length) {
				currPage.hasNextPage = false;
			}
			ata_write(disk, &currPage, sizeof(FilePage), page->nextSector, page->nextOffset);
			currPage.usedBytes = MIN(currPage.totalLength - sizeof(FilePage) - sizeof(FileHeader), length);
			ata_write(disk, contents, currPage.usedBytes, page->nextSector, page->nextOffset + sizeof(FilePage));
			contents += currPage.usedBytes;
			length -= currPage.usedBytes;
		} else {
			errno = E_CORRUPTED_FILE;
			return -1;
		}
	}
	return 0;
	/*FilePage currPage;
	currPage.magic = MAGIC_NUMBER;
	currPage.hasNextPage = false;
	printf("writing filepage [%d,%d]s: %d\n", page->nextSector, page->nextOffset, sizeof(FilePage));
	ata_write(disk, &currPage, sizeof(FilePage), page->nextSector, page->nextOffset);
	u32int offset = sizeof(FilePage);
	printf("writing header [%d,%d]s: %d\n", page->nextSector, page->nextOffset + offset, sizeof(FileHeader));
	printf("flags: %d\n", header->flags);
	ata_write(disk, header, sizeof(FileHeader), page->nextSector, page->nextOffset + offset);
	offset += sizeof(FileHeader);
	u32int written = MIN(page->totalLength, length);
	ata_write(disk, contents, written, page->nextSector, page->nextOffset + offset);	// Fill page with contents
	printf("writing contents [%d, %d]s: %d / %s\n", page->nextSector, page->nextOffset + offset, written, contents);
	contents += written;
	FilePage nextPage;
	while (currPage.hasNextPage && written < length) {
		ata_read(disk, &nextPage, sizeof(FilePage), currPage.nextSector, currPage.nextOffset);
		int write = MIN(currPage.totalLength, length);
		ata_write(disk, contents, write, currPage.nextSector, currPage.nextOffset + sizeof(FilePage));
		printf("writing contents [%d, %d]s: %d / %s\n", currPage.nextSector, currPage.nextOffset + sizeof(FilePage), write, contents);
		contents += write;
		written += write;
		memcpy(&currPage, &nextPage, sizeof(FilePage));	// Set next page as current page
	}
	return written;*/
}

PRIVATE void _reserveMemory(FilePage *page, int size, u32int initialSector, u32int initialOffset) {
	FilePage currPage;
	int sector = initialSector;
	int offset = initialOffset;
	int index = 0;
	int previousSector, previousOffset;

	u32int maxOffset = 4098;	// FIXME: implement driveCapacity() in the ata_driver

	int blocks = (size / (FILE_BLOCK_SIZE_BYTES + 1)) + 1;
	while (index < blocks && offset < maxOffset) {
		ata_read(currDisk, &currPage, sizeof(FilePage), sector, offset);
		//printf("[%d, %d] -> %d\n", sector, offset, currPage.magic);
		if (currPage.magic != MAGIC_NUMBER) {										// The block is empty... can be used
			currPage.magic = MAGIC_NUMBER;
			currPage.hasNextPage = false;
			currPage.usedBytes = 0;
			currPage.totalLength = FILE_BLOCK_SIZE_BYTES;
			ata_write(currDisk, &currPage, sizeof(FilePage), sector, offset);		// write header to disk
			if (index > 0) {														// Set previous header to point to this one
				ata_read(currDisk, &currPage, sizeof(FilePage), previousSector, previousOffset);
				currPage.nextSector = sector;
				currPage.nextOffset = offset;
				currPage.hasNextPage = true;
				ata_write(currDisk, &currPage, sizeof(FilePage), previousSector, previousOffset);
			} else {																// save first page to return
				//printf("asigning: %d, %d\n", sector, offset);
				page->nextSector = sector;
				page->nextOffset = offset;
				page->usedBytes = 0;
				page->totalLength = FILE_BLOCK_SIZE_BYTES;
				page->magic = MAGIC_NUMBER;
			}
			previousSector = sector;
			previousOffset = offset;
			index++;
		}
		offset += FILE_BLOCK_SIZE_BYTES;
	}
	if (offset >= maxOffset) {			// Reached end of space available and the recolected spaace is not enought
		printf("freeing memory...\n");
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
	}
}

PRIVATE void _setiNode(u32int inode, FilePage *page) {
	int sector = 1;
	// printf("_setiNode %d node: [%d, %d], size: %d\n", inode, sector, inode * sizeof(FilePage), sizeof(FilePage));
	ata_write(currDisk, page, sizeof(FilePage), sector, inode * sizeof(FilePage));
}

PRIVATE void _getiNode(int inode, FilePage *page) {
	int sector = 1;
	ata_read(currDisk, page, sizeof(FilePage), sector, inode * sizeof(FilePage));
	//printf("_getiNode %d node: [%d, %d], size: %d\n", inode, page->nextSector, page->nextOffset, page->totalLength);
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

void _getFileheader(int inode, FileHeader *header) {
	FilePage page;
	_getiNode(inode, &page);
	if (page.magic != MAGIC_NUMBER) {
		errno = E_CORRUPTED_FILE;
		return;
	}
	ata_read(currDisk, header, sizeof(FileHeader), page.nextSector, page.nextOffset + sizeof(FilePage));
}

void _setFileheader(int inode, FileHeader *header) {
	FilePage page;
	_getiNode(inode, &page);
	if (page.magic != MAGIC_NUMBER) {
		errno = E_CORRUPTED_FILE;
		return;
	}
	ata_write(currDisk, header, sizeof(FileHeader), page.nextSector, page.nextOffset + sizeof(FilePage));
}
