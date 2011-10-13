#include <fs/diskManager.h>

#define MIN(x,y)							((x) < (y)) ? (x) : (y)

#define FILE_CONTENTS_INITAL_SECTOR			1
#define FILE_CONTENTS_INITAL_OFFSET			maxIndes * sizeof(FilePage)

PRIVATE u32int maxIndes;						// Once setted this value, it should never be changed!!

PRIVATE u32int currDisk;
PRIVATE u32int currSector;
PRIVATE u32int currOffset;

PRIVATE void _free(u32int sector, u32int offset);
PRIVATE void _loadContents(u32int sector, u32int offset, char* target, int bytes);
PRIVATE int _writeContents(u32int sector, u32int offset, char* target, int bytes);
PRIVATE void _findHole(FilePage* page, int size, u32int initialSector, u32int initialOffset);

void diskManager_init() {
	currDisk = ATA0;
	currSector = 1;		// Start working at sector 1
	currOffset = 0;
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

void diskManager_writeiNode(iNode* inode, int inodeNumber) {
	int sector = 1;
	u32int offset = inodeNumber * sizeof(FilePage);
	u32int disk = currDisk;

	FilePage page;
	ata_read(disk, &page, sizeof(FilePage), sector, offset);

	if (inode->length == 0) {				// inode has no loaded contents
		if (page.hasContents) {				//page has reserved disk space
			_free(page.sector, page.offset);
		}
		page.hasContents = false;
		ata_write(disk, &page, sizeof(FilePage), sector, offset);
		return;
	}
	// FIXME: doing this for every file is a bit inneficient... some valdiation here could help improve performance
	_free(page.sector, page.offset);
	_findHole(&page, inode->length, FILE_CONTENTS_INITAL_SECTOR, FILE_CONTENTS_INITAL_OFFSET);
	_writeContents(page.sector, page.offset, inode->contents, inode->length);
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
	_loadContents(inode->sector, inode->offset, inode->contents, inode->length);
	return 0;
}

PRIVATE void _free(u32int sector, u32int offset) {
	FileHeader fileHeader;
	u32int disk = currDisk;
	do {
		ata_read(disk, &fileHeader, sizeof(FileHeader), sector, offset);
		sector = fileHeader.nextSector;
		offset = fileHeader.nextOffset;
		fileHeader.magic = 0;
		ata_write(disk, &fileHeader, sizeof(FileHeader), sector, offset);
	} while(fileHeader.hasNextPage);
}

PRIVATE void _loadContents(u32int sector, u32int offset, char* target, int bytes) {
	FileHeader header;
	u32int disk = currDisk;
	do {
		ata_read(disk, &header, sizeof(FileHeader), sector, offset);
		if (header.magic == MAGIC_NUMBER) {
			ata_read(disk, target, header.usedBytes, sector, offset);
			sector = header.nextSector;
			offset = header.nextOffset;
			target += header.usedBytes;
		} else {
			errno = E_CORRUPTED_FILE;
			return;
		}
	} while(header.hasNextPage);
}

PRIVATE int _writeContents(u32int sector, u32int offset, char* target, int bytes) {
	FileHeader header;
	int disk = currDisk;
	int writtenBytes = 0;
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

PRIVATE void _findHole(FilePage* page, int size, u32int initialSector, u32int initialOffset) {
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
			}
			previousSector = sector;
			previousOffset = offset;
			index++;
		}
		offset += FILE_BLOCK_SIZE_BYTES;
	}
	if (offset >= maxOffset) {			// Reached end of space available and the recolected spaace is not enought
		page->totalLength = 0;
		_free(page->sector, page->offset);
		errno = E_OUT_OF_MEMORY;
	}
}

