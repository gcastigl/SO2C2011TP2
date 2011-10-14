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

PRIVATE int _readContents(FilePage* page, char* target, int bytes);
PRIVATE int _writeContents(FilePage* page, char* target, int bytes);

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

int diskManager_createiNode() {
	FSHeader header;
	ata_read(currDisk, &header, sizeof(FSHeader), 0, 0);
	int nextiNode = header.totalNodes++;

	FilePage page;
	_reserveMemory(&page, 100, FILES_INITIAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	_updateiNode(nextiNode, &page);

	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);			// Update totalNodes value
	return nextiNode;
}

void diskManager_updateiNodeContents(u32int inodeNumber, char* contents, u32int length) {
	FilePage page;
	_getiNode(inodeNumber, &page);

	if (length == 0) {				// inode has no loaded contents
		_freeMemory(&page);
		_updateiNode(inodeNumber, &page);
		return;
	}
	// FIXME: doing this for every file is a bit inneficient... some valdiation here could help improve performance
	_freeMemory(&page);
	_reserveMemory(&page, length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	_writeContents(&page, contents, length);
}

int diskManager_readiNode(iNode* inode, int inodeNumber, int mode) {
	int sector = 1;
	int offset = inodeNumber * sizeof(FilePage);
	int disk = currDisk;

	FilePage page;
	ata_read(disk, &page, sizeof(FilePage), sector, offset);

	inode->sector = page.sector;
	inode->offset = page.offset;
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
	_readContents(&page, inode->contents, inode->length);
	return 0;
}

PRIVATE void _freeMemory(FilePage* page) {
	FileHeader fileHeader;
	u32int disk = currDisk;
	u32int sector = page->sector;
	u32int offset = page->offset;
	do {
		ata_read(disk, &fileHeader, sizeof(FileHeader), sector, offset);
		sector = fileHeader.nextSector;
		offset = fileHeader.nextOffset;
		fileHeader.magic = 0;
		ata_write(disk, &fileHeader, sizeof(FileHeader), sector, offset);
	} while(fileHeader.hasNextPage);
}

PRIVATE int _readContents(FilePage* page, char* target, int bytes) {
	FileHeader header;
	u32int disk = currDisk;
	u32int sector = page->sector;
	u32int offset = page->offset;
	do {
		ata_read(disk, &header, sizeof(FileHeader), sector, offset);
		if (header.magic == MAGIC_NUMBER) {
			ata_read(disk, target, header.usedBytes, sector, offset);
			sector = header.nextSector;
			offset = header.nextOffset;
			target += header.usedBytes;
		} else {
			errno = E_CORRUPTED_FILE;
			return 0;
		}
	} while(header.hasNextPage);
	return 0;
}

PRIVATE int _writeContents(FilePage* page, char* target, int bytes) {
	FileHeader header;
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
	return writtenBytes;
}

PRIVATE void _reserveMemory(FilePage* page, int size, u32int initialSector, u32int initialOffset) {
	FileHeader fileHeader;
	int sector = initialSector;
	int offset = initialOffset;
	int index = 0;

	int previousSector, previousOffset;

	u32int maxOffset = 4098;// FIXME: implement driveCapacity() in the ata_driver

	int neededPages = (size / (FILE_BLOCK_SIZE_BYTES + 1)) + 1;
	while (index < neededPages && offset < maxOffset) {
		ata_read(currDisk, &fileHeader, sizeof(FileHeader), sector, offset);
		//printf("[%d, %d] -> %d\n", sector, offset, fileHeader.magic);
		if (fileHeader.magic != MAGIC_NUMBER) {											// The block is empty... can be used
			fileHeader.magic = MAGIC_NUMBER;
			fileHeader.hasNextPage = false;
			fileHeader.usedBytes = 0;
			fileHeader.maxBytes = FILE_BLOCK_SIZE_BYTES - sizeof(FileHeader);
			ata_write(currDisk, &fileHeader, sizeof(FileHeader), sector, offset);		// write header to disk
			if (index > 0) {															// Set previous header to point to this one
				fileHeader.nextSector = sector;
				fileHeader.nextOffset = offset;
				fileHeader.hasNextPage = true;
				ata_write(currDisk, &fileHeader, sizeof(FileHeader), previousSector, previousOffset);
			} else {																	// save first page to return
				page->sector = sector;
				page->offset = offset;
				page->isDeleted = false;
				page->totalLength = size;
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
