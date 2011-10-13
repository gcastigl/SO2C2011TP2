#include <fs/fs.h>

static fs_node_t *root;             // Our root directory node.
static iNode *inodes;				// List of file nodes.

void fs_create();
void fs_load();

void loadInode(int inodeNumber, iNode* inode) ;

static struct dirent* fs_readdir(fs_node_t *fsnode, u32int index);
static struct fs_node* fs_finddir(struct fs_node *fsnode, char *name);

static u32int fs_read(struct fs_node *fsnode, u32int offset, u32int size, u8int *buffer);
static u32int fs_write(struct fs_node *fsnode, u32int offset, u32int size, u8int *buffer);

static void fs_open(struct fs_node* fsnode);
static void fs_close(struct fs_node* fsnode);

void fs_init() {
	diskManager_init(INODES);
	if (false && diskManager_validateHeader()) {
		fs_load();
	} else {
		fs_create();
	}
}

void fs_create() {
	diskManager_writeHeader();				// Save header for the next time the system starts...

	inodes = kmalloc(INODES * sizeof(iNode));
	// Initialize root directory
	root = (fs_node_t*) kmalloc(sizeof(fs_node_t));
    strcpy(root->name, "~");
    root->inode = 0;
    root->mask = root->uid = root->gid = root->length = 0;
    root->flags = FS_DIRECTORY;
    root->read = 0;
    root->write = 0;
    root->open = 0;
    root->close = 0;
    root->readdir = fs_readdir;
    root->finddir = fs_finddir;
    root->ptr = 0;
    root->impl = 0;

    inodes[0].contents = NULL;
   // inodes[0].length = 0;
    diskManager_writeiNode(&inodes[0], 0);
}


fs_node_t* fs_getRoot() {
	return root;
}


void fs_load() {
	// TODO: hacer!
}

void loadInode(int inodeNumber, iNode* inode) {
}

//==================================================================
//	callbacks - called when read/write/open/close are called.
//==================================================================

static struct dirent* fs_readdir(fs_node_t *fsnode, u32int index) {
	// 123 .'\0'
	// 247 ..'\0'
	// 260 pepe'\0'
	// 401 memos'\0'
	struct dirent* dirent = (struct dirent*) kmalloc(sizeof(struct dirent));
	char* contents = inodes[fsnode->inode].contents;
	u32int i = 0;
	while (i != index + 3) {					// Do not read first 2 strings...
		memcpy(&dirent->ino, contents, sizeof(u32int));
		contents += sizeof(u32int);
		int len = strlen(contents);
		contents += len;
		i++;
	}
	return dirent;
}

static struct fs_node* fs_finddir(struct fs_node *fsnode, char *name) {
	return NULL;
}

static u32int fs_read(struct fs_node *fsnode, u32int offset, u32int size, u8int *buffer) {
	return 0;
}

static u32int fs_write(struct fs_node *fsnode, u32int offset, u32int size, u8int *buffer) {
	return 0;
}

static void fs_open(struct fs_node* fsnode) {
	/*
	iNode inode = inodes[fsnode->inode];
	FileHeader header;
	if(inode.contents == NULL) {
		u32int sector = inode.sector;
		u32int offset = inode.offset;
		while (sector != -1) {
			ata_read(currDisk, &header, sizeof(FileHeader), sector, offset);
			if (header.magic != MAGIC_NUMBER) {
				// TODO: set errno to E_CORRUPTED_FILE!!
				return;
			}
			// FIXME: here there should be a realloc(not implemented)!!
			inode.contents = kmalloc(header.length);
			ata_read(currDisk, inode.contents, header.length, sector, offset);
			sector = header.nextSector;
			offset = header.nextOffset;
		}
	}*/
}

static void fs_close(struct fs_node* fsnode) {

}









// ==============================================================
//			FS persiatnce
// ==============================================================

/*


#define FILE_TABLE_INIT_SECTOR		1024
#define FILE_TABLE_FINAL_SECTOR		4096

// =======================================================
// Functions used to persist files and folders on disk
// =======================================================

void persist|ctory(Directory* dir);
void persist(char* string, int size);

char* serializeDirectory(Directory* dir, int* finalSize);
char* serializeFile(iNode* file, int* finalSize);

void parseDirectories(Directory* current);

void unserializeDirectory(char* name, u32int* childs);
iNode* unserializeFile(Directory* folder);

static void findHole(FilePage* page, int size);


int fs_createDirectory(Directory* parent, char* name) {
	int created = directory_createDir(parent, name);
	if (created != 0) {			// There was an error creating the directory
		return created;
	}
	// Save changes to disk
	currSector = 1;
	currOffset = 0;
	persistDirectory(directory_getRoot());
	return 0;
}

int fs_createFile(Directory* parent, char* name) {
	int created = directory_createFile(parent, name);
	if (created != 0) {			// There was an error creating the file
		return created;
	}
	FilePage page;
	findHole(&page, 50);
	if (page.sector == (u32int) -1) {		// No more memory available
		return E_OUT_OF_MEMORY;
	}
	int fileIndex = parent->fileTableEntry->filesCount - 1;
	parent->fileTableEntry->files[fileIndex]->sector = page.sector;
	parent->fileTableEntry->files[fileIndex]->offset = page.offset;
	parent->fileTableEntry->files[fileIndex]->contents = NULL;
	parent->fileTableEntry->files[fileIndex]->used = false;
	return 0;
}

// ==============================================================
//			FS persiatnce
// ==============================================================

// Funcion recursiva que, dado un directorio, guarda en disco tod su contenido
// a partir de la posicion currSector y currOffset
void persistDirectory(Directory* dir) {
	int size = 0;
	int i;
	char* serializedDir = serializeDirectory(dir, &size);
	persist(serializedDir, size);
	kfree(serializedDir);
	for(i = 0; i < dir->subDirsCount; i++) {
		persistDirectory(dir->subDirs[i]);
	}
}

char* serializeDirectory(Directory* dir, int* finalSize) {
	char* serial = (char*) kmalloc(MAX_FILENAME_LENGTH + sizeof(u32int));
	int offset = 0;
	memcpy(serial + offset, &dir->subDirsCount, sizeof(u32int)); 	offset += sizeof(u32int);
	memcpy(serial + offset, dir->name, MAX_FILENAME_LENGTH);		offset += MAX_FILENAME_LENGTH;
	*finalSize = offset;
	return serial;
}

void persist(char* string, int size) {
	//printf("saving from %d byted at: (%d, %d) to (%d, %d)\n", size, currSector, currOffset, currSector, currOffset + size);
	ata_write(currDisk, string, size, currSector, currOffset);
	currOffset += size;
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
	directory_initialize();
	Directory* root = directory_getRoot();
	parseDirectories(root);
}

// Funcion recursiva que se encarga de leer el arbol de directorios del disco a partir de la posicion actual
// de currSector y currSector.
void parseDirectories(Directory* current) {
	char name[MAX_FILENAME_LENGTH];
	u32int i, childs;

	unserializeDirectory(name, &childs);
	//printf("parsed: %s - %d --- Saving to: %d\n", name, childs, current);
	initEmptyDirectory(current, name);
	for(i = 0; i < childs; i++) {
		current->subDirs[i] = (Directory*) kmalloc(sizeof(Directory));
		parseDirectories(current->subDirs[i]);
		current->subDirs[i]->parent = current;
	}
	current->subDirsCount = childs;
}

// Lee del disco un directorio desde la posicion actual de currSector y currOffset y lo guarda en dir.
void unserializeDirectory(char* name, u32int* childs) {
	// int bytes = MAX_FILENAME_LENGTH + sizeof(u32int);
	// printf("reading %d bytes at: (%d, %d) to (%d, %d)\n", bytes, currSector, currOffset, currSector, currOffset + bytes);
	ata_read(currDisk,  childs, sizeof(u32int), currSector, currOffset);		currOffset += sizeof(u32int);
	ata_read(currDisk, name, MAX_FILENAME_LENGTH, currSector, currOffset);		currOffset += MAX_FILENAME_LENGTH;
}

iNode* unserializeFile(Directory* folder) {
	iNode* file = (iNode*) kmalloc(sizeof(iNode));
	file->sector = currSector;
	file->offset = currOffset;
	ata_read(currDisk, file->name, MAX_FILENAME_LENGTH, currSector, currOffset); 		currOffset += MAX_FILENAME_LENGTH;
	ata_read(currDisk, &file->contentsLength, sizeof(u32int), currSector, currOffset); 	currOffset += sizeof(u32int);
	file->contents = (char*) kmalloc(file->contentsLength);
	ata_read(currDisk, file->contents, sizeof(u32int), currSector, currOffset); 		currOffset += file->contentsLength;
	return file;
}



void fs_format() {
	FileHeader fileHeader;
	fileHeader.magic = FILE_MAGIC_NUMBER + 1; // Always different from the FILE_MAGIC_NUMBER!
	int sector = FILE_TABLE_INIT_SECTOR;
	int offset = 0;
	// Delete FS header
	ata_write(ATA0, "000000", 6, 0, 0);
	int times = 0;
	while(times < 100) {
		ata_write(ATA0, &fileHeader, sizeof(FileHeader), sector, offset);
		offset += FILE_TABLE_INIT_SECTOR;
		times++;
	}
}
*/



