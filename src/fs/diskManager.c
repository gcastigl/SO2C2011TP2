#include <fs/diskManager.h>

static u32int currDisk;
static u32int currSector;
static u32int currOffset;

static void findHole(FilePage* page, int size, u32int initialSector, u32int initialOffset);

void diskManager_init() {
	currDisk = ATA0;
	currSector = 1;		// Start working at sector 1
	currOffset = 0;
}

boolean diskManager_validateHeader() {
	FSHeader header;
	ata_read(currDisk, &header, sizeof(FSHeader), 0, 0);
	return header.magic == MAGIC_NUMBER;
}

void diskManager_writeHeader() {
	FSHeader header;
	header.magic = 123456;
	header.totalNodes = 0;
	ata_write(currDisk, &header, sizeof(FSHeader), 0, 0);
}

int diskManager_writeiNode(iNode* inode) {
	FilePage page;
	FileHeader fileHeader;

	ata_read(currDisk, &fileHeader, sizeof(FileHeader), inode->sector, inode->offset);	// Read contents
	if (fileHeader.magic != MAGIC_NUMBER) {
		return E_CORRUPTED_FILE;
	}
	if (inode->length > fileHeader.length) { // Space in this sector is not long enough for new contents
		fileHeader.magic = 0;
		ata_write(currDisk,&fileHeader, sizeof(FileHeader), inode->sector, inode->offset);
		findHole(&page, inode->length, FILES_INITIAL_SECTOR, 0);
	} else {								// Space is enough, use the same page
		page.sector = inode->sector;
		page.offset = inode->offset;
	}
	ata_write(currDisk, inode->contents, inode->length, page.sector, page.offset);
	return 0;
}

// Busca desde el sector FILE_TABLE_INIT_SECTOR hasta el FILE_TABLE_FINAL_SECTOR por un espacio vacio de size
// bytes para crear un archivo.
// De encontrarse setea header con los valors adecuados. Sino setea a sector con -1.
static void findHole(FilePage* page, int size, u32int initialSector, u32int initialOffset) {
	FileHeader fileHeader;	// This is the disk address on disk where to file will be written
	int sector = initialSector;
	int offset = initialOffset;
	int index = 0;
	int previousSector, previousOffset;		// auxiliary variables
	u32int maxOffset = 4098;			// FIXME: implement driveCapacity() in the ata_driver

	int neededPages = (size / (FILE_BLOCK_SIZE_BYTES + 1)) + 1;
	while (index < neededPages && offset < maxOffset) {
		ata_read(currDisk, &fileHeader, sizeof(FileHeader), sector, offset);
		//printf("[%d, %d] -> %d\n", sector, offset, fileHeader.magic);
		if (fileHeader.magic != MAGIC_NUMBER) {					// The block is empty... can be used
			fileHeader.magic = MAGIC_NUMBER;
			fileHeader.nextSector = -1;
			fileHeader.length = FILE_BLOCK_SIZE_BYTES - sizeof(FileHeader);
			//printf("using this sector for page %d / %d\n", index, neededPages);
			ata_write(currDisk, &fileHeader, sizeof(FileHeader), sector, offset);			// write header to disk
			if (index > 0) {											// Set previous header to point to this one
				fileHeader.nextSector = sector;
				fileHeader.nextOffset = offset;
				ata_write(currDisk, &fileHeader, sizeof(FileHeader), previousSector, previousOffset);
			} else {													// save first page to return
				page->sector = sector;
				page->offset = offset;
			}
			previousSector = sector;
			previousOffset = offset;
			index++;
		}
		offset += FILE_BLOCK_SIZE_BYTES;
	}
	if (offset >= maxOffset) {	// Reached end of space available and the number of pages found is less that the required
		// FIXME: free reserved blocks!!
		page->sector = -1;
	}
}
