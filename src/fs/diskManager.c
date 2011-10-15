#include <fs/diskManager.h>

#define MIN(x,y)							((x) < (y)) ? (x) : (y)

#define FILE_CONTENTS_INITAL_SECTOR			1
#define FILE_CONTENTS_INITAL_OFFSET			maxIndes * sizeof(FilePage)

PRIVATE u32int maxIndes;						// Once setted this value, it should never be changed!!

PRIVATE u32int currDisk;


PRIVATE void _getiNode(int inode, FilePage* contents);
PRIVATE void _updateiNode(int inode, FilePage* contents);

PRIVATE void _reserveMemory(FilePage* page, int size, u32int initialSector, u32int initialOffset);
PRIVATE void _freeMemory(FilePage* page);

PRIVATE int _readContents(FilePage* page, iNode* inode);
PRIVATE int _writeContents(FilePage* page, iNode* inode);

void diskManager_init() {
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
	header.magic = 123456;
	header.totalNodes = 0;
	header.maxNodes = maxNodes;
	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);
}

int diskManager_nextInode() {
	FSHeader header;
	ata_read(currDisk, &header, sizeof(FSHeader), 0, 0);
	int nextiNode = header.totalNodes++;
	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);			// Update totalNodes value
	return nextiNode;
}

void diskManager_updateiNodeContents(iNode* inode, u32int inodeNumber) {
	FilePage page;
	_getiNode(inodeNumber, &page);

	// FIXME: doing this for every file is a bit inneficient... some valdiation here could help improve performance
	_freeMemory(&page);
	_reserveMemory(&page, sizeof(FSHeader) + inode->length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	_writeContents(&page, inode);
}

int diskManager_readiNode(iNode* inode, int inodeNumber, int mode) {
	FilePage page;
	_getiNode(inodeNumber, &page);

	// FIXME: actualizar la logica de esta funcion!
	if (mode == MODE_NO_CONTENTS) {
		inode->length = 0;
		inode->contents = NULL;
		return 0;
	}
	if (mode == MODE_ALL_CONTENTS) {
		inode->length = page.totalLength;
	} else {
		inode->length = mode;
	}
	inode->contents = kmalloc(inode->length);
	_readContents(&page, inode);
	return 0;
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

PRIVATE int _readContents(FilePage* page, iNode* inode) {
	FileHeader header;
	FilePage currPage;
	u32int disk = currDisk;

	u32int sector = page->nextSector;
	u32int offset = page->nextOffset;
	ata_read(disk, &currPage, sizeof(FilePage), sector, offset);
	offset += sizeof(FilePage);
	if (currPage.magic != MAGIC_NUMBER) {
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	// Read FileHeader fisrt...
	ata_read(disk, &header, sizeof(FileHeader), sector, offset);
	if (header.magic != MAGIC_NUMBER) {
		errno = E_CORRUPTED_FILE;
		return -1;
	}
	strcpy(inode->name, header.name);
	inode->flags = header.flags;
	inode->uid = header.uid;
	inode->gid = header.gid;
	inode->impl = header.impl;
	inode->mask = header.mask;
	char *p = inode->contents;
	do {
		// TODO: revisar bien la logica...
		ata_read(disk, &currPage, sizeof(FilePage), currPage.nextSector, currPage.nextOffset);
		offset += sizeof(FilePage);
		ata_read(disk, p, currPage.usedBytes, sector, offset);
		sector = currPage.nextSector;
		offset = currPage.nextOffset;
		p += currPage.usedBytes;
	} while(currPage.hasNextPage);
	return 0;
}

PRIVATE int _writeContents(FilePage* page, iNode* inode) {
	// FIXME: modificar para que funcione con los nuevos cambios
	/*FileHeader header;
	int disk = currDisk;
	int writtenBytes = 0;
	u32int sector = page->sector;
	u32int offset = page->offset;
	do {
		ata_read(disk, &header, sizeof(FileHeader), sector, offset);
		if (header.magic == MAGIC_NUMBER) {
			int writeBytes = MIN(header.maxBytes, bytes);
			ata_write(disk, target, writeBytes, sector, offset);
			// update header...
			header.usedBytes = writeBytes;
			ata_write(disk, &header, sizeof(FileHeader), sector, offset);

			sector = header.nextSector;
			offset = header.nextOffset;
			bytes -= writeBytes;
			writtenBytes += writeBytes;
		} else {
			errno = E_CORRUPTED_FILE;
			return writtenBytes;
		}
	} while (header.hasNextPage && bytes != 0);
	return writtenBytes;*/
	return 0;
}

PRIVATE void _reserveMemory(FilePage* page, int size, u32int initialSector, u32int initialOffset) {
	FilePage currPage;
	int sector = initialSector;
	int offset = initialOffset;
	int index = 0;

	int previousSector, previousOffset;

	u32int maxOffset = 4098;	// FIXME: implement driveCapacity() in the ata_driver

	int neededPages = (size / (FILE_BLOCK_SIZE_BYTES + 1)) + 1;
	while (index < neededPages && offset < maxOffset) {
		ata_read(currDisk, &currPage, sizeof(FilePage), sector, offset);
		//printf("[%d, %d] -> %d\n", sector, offset, fileHeader.magic);
		if (currPage.magic != MAGIC_NUMBER) {											// The block is empty... can be used
			currPage.magic = MAGIC_NUMBER;
			currPage.hasNextPage = false;
			currPage.usedBytes = 0;
			currPage.totalLength = FILE_BLOCK_SIZE_BYTES - sizeof(FilePage);
			ata_write(currDisk, &currPage, sizeof(FilePage), sector, offset);		// write header to disk
			if (index > 0) {															// Set previous header to point to this one
				currPage.nextSector = sector;
				currPage.nextOffset = offset;
				currPage.hasNextPage = true;
				ata_write(currDisk, &currPage, sizeof(FilePage), previousSector, previousOffset);
			} else {																	// save first page to return
				page->nextSector = sector;
				page->nextOffset = offset;
				page->usedBytes = 0;
				page->totalLength = FILE_BLOCK_SIZE_BYTES - sizeof(FilePage);
			}
			previousSector = sector;
			previousOffset = offset;
			index++;
		}
		offset += FILE_BLOCK_SIZE_BYTES;
	}
	if (offset >= maxOffset) {			// Reached end of space available and the recolected spaace is not enought
		page->totalLength = 0;
		_freeMemory(page);
		errno = E_OUT_OF_MEMORY;
	}
}

PRIVATE void _updateiNode(int inode, FilePage* contents) {
	int sector = 1;
	ata_write(currDisk, contents, sizeof(FilePage), sector, inode * sizeof(FilePage));
}

PRIVATE void _getiNode(int inode, FilePage* contents) {
	int sector = 1;
	ata_read(currDisk, contents, sizeof(FilePage), sector, inode * sizeof(FilePage));
}
