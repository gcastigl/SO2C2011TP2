#include <fs.h>
#include <driver/video.h>

#define FILE_TABLE_SECTOR		1024

static iNode iNodes[MAX_INODES];

static u32int currDisk;
static u32int currSector;
static u32int currOffset;

void fs_create();
void fs_load();

boolean validate_header();
void write_header();

// =======================================================
// Functions used to persist files and folders on disk
// =======================================================

void persistDirectory(Directory_t* dir);
void persist(char* string, int size);

char* serializeDirectory(Directory_t* dir, int* finalSize);
char* serializeFile(iNode* file, int* finalSize);

void parseDirectories(Directory_t* dir, Directory_t* parent);

void unserializeDirectory(Directory_t* dir, Directory_t* parent);
iNode* unserializeFile(Directory_t* folder);

void updateNumberOfFoldersOnDisk(boolean increment);

void fs_init() {
	currDisk = ATA0;
	currSector = 1;		// Start working at sector 1
	currOffset = 0;
	if (false && validate_header()) {
		fs_load();
	} else {
		fs_create();
	}
}

boolean validate_header() {
	int len = strlen(FS_HEADER);
	char header[len];
	ata_read(currDisk, header, len, 0, 0);
	return strcmp(header, FS_HEADER) == 0;
}

void write_header() {
	ata_write(currDisk, FS_HEADER, strlen(FS_HEADER), 0, 0);
}

void fs_create() {
	write_header();				// Save header for the next time the system starts...
	int i;
	for(i = 0; i < MAX_INODES; i++) {
		iNodes[i].contents = NULL;
	}
	// create root and save it to disk
	directory_initialize();
	// create /dev
	fs_createDirectory(directory_getRoot(), "dev");
}

int fs_createDirectory(Directory_t* parent, char* name) {
	int created = directory_createDir(parent, name);
	if (created != 0) {			// There was an error creating the directory
		return created;
	}
	currSector = 1;
	currOffset = 0;
	// Save changes to disk
	persistDirectory(directory_getRoot());
	updateNumberOfFoldersOnDisk(true);
	return 0;
}

int fs_createFile(Directory_t* parent, char* name) {
	return 0;
}

// ==============================================================
//			FS persiatnce
// ==============================================================

// Funcion recursiva que, dado un directorio, guarda en disco todo su contenido
// a partir de la posicion currSector y currOffset
void persistDirectory(Directory_t* dir) {
	int size = 0;
	int i;
	char* serializedDir = serializeDirectory(dir, &size);
	persist(serializedDir, size);
	kfree(serializedDir);
	for(i = 0; i < dir->subDirsCount; i++) {
		persistDirectory(dir->subDirs[i]);
	}
}

char* serializeDirectory(Directory_t* dir, int* finalSize) {
	char* serial = (char*) kmalloc(2 * MAX_FILENAME_LENGTH + 2 * sizeof(u32int));
	int offset = 0;
	int header = FS_FILE;
	memcpy(serial + offset, &header, sizeof(u32int)); 	offset += sizeof(u32int);
	memcpy(serial + offset, &dir->subDirsCount, sizeof(u32int)); 	offset += sizeof(u32int);
	memcpy(serial + offset, dir->name, MAX_FILENAME_LENGTH);		offset += MAX_FILENAME_LENGTH;
	if (dir->parent == NULL) {
		char nullName[MAX_FILENAME_LENGTH]; nullName[0] = '\0';
		memcpy(serial + offset, nullName, MAX_FILENAME_LENGTH);
	} else {
		memcpy(serial + offset, dir->parent->name, MAX_FILENAME_LENGTH);
	}
	offset += MAX_FILENAME_LENGTH;
	*finalSize = offset;
	return serial;
}

void persist(char* string, int size) {
	//printf("saving from %d byted at: (%d, %d) to (%d, %d)\n", size, currSector, currOffset, currSector, currOffset + size);
	ata_write(currDisk, string, size, currSector, currOffset);
	currOffset += size;
}

void updateNumberOfFoldersOnDisk(boolean increment) {
	u32int currnumberOfDirs;
	ata_read(currDisk, &currnumberOfDirs, sizeof(u32int), 1, 0);
	if (increment)	currnumberOfDirs++;
	else currnumberOfDirs --;
	ata_write(currDisk, &currnumberOfDirs, sizeof(u32int), 1, 0);
}

char* serializeFile(iNode* file, int* finalSize) {
	char* serial = (char*) kmalloc(MAX_FILENAME_LENGTH + sizeof(u32int) + file->contentsLength);
	int offset = 0;
	memcpy(serial + offset, file->name, MAX_FILENAME_LENGTH); 			offset += MAX_FILENAME_LENGTH;
	memcpy(serial + offset, &file->contentsLength, sizeof(u32int)); 	offset += sizeof(u32int);
	memcpy(serial + offset, file->contents, file->contentsLength); 		offset += file->contentsLength;
	*finalSize = offset;
	return serial;
}

void fs_load() {
	currOffset += sizeof(u32int);
	directory_initialize();
	Directory_t* root = directory_getRoot();
	parseDirectories(root, NULL);
}

// Funcion recursiva que se encarga de leer el arbol de directorios del disco a partir de la posicion actual
// de currSector y currSector.
void parseDirectories(Directory_t* dir, Directory_t* parent) {
	u32int i;
	unserializeDirectory(dir, parent);
	printf("%s -> %d childs\n", dir->name, dir->subDirsCount);
	for(i = 0; i < dir->subDirsCount; i++) {
		dir->subDirs[i] = (Directory_t*) kmalloc(sizeof(Directory_t));
		parseDirectories(dir->subDirs[i], dir);
	}
}

// Lee del disco un directorio desde la posicion actual de currSector y currOffset y lo guarda en dir.
void unserializeDirectory(Directory_t* dir, Directory_t* parent) {
	printf("reading %d bytes at: (%d, %d) to (%d, %d)\n", 68, currSector, currOffset, currSector, currOffset + 68);
	dir->parent = parent;
	char parentName[MAX_FILENAME_LENGTH];		// Not used for now sine we already know the parent
	ata_read(currDisk,  &dir->subDirsCount, sizeof(u32int), currSector, currOffset);	currOffset += sizeof(u32int);
	ata_read(currDisk, dir->name, MAX_FILENAME_LENGTH, currSector, currOffset);		currOffset += MAX_FILENAME_LENGTH;
	ata_read(currDisk, parentName, MAX_FILENAME_LENGTH, currSector, currOffset);	currOffset += MAX_FILENAME_LENGTH;
}

iNode* unserializeFile(Directory_t* folder) {
	iNode* file = (iNode*) kmalloc(sizeof(iNode));
	file->sector = currSector;
	file->offset = currOffset;
	ata_read(currDisk, file->name, MAX_FILENAME_LENGTH, currSector, currOffset); 		currOffset += MAX_FILENAME_LENGTH;
	ata_read(currDisk, &file->contentsLength, sizeof(u32int), currSector, currOffset); 	currOffset += sizeof(u32int);
	file->contents = (char*) kmalloc(file->contentsLength);
	ata_read(currDisk, file->contents, sizeof(u32int), currSector, currOffset); 		currOffset += file->contentsLength;
	return file;
}

