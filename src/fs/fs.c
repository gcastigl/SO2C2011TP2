#include <fs/fs.h>

PRIVATE fs_node_t root;             // Our root directory node.
PRIVATE iNode *inodes;				// List of file nodes.

PRIVATE void fs_create();
PRIVATE void fs_load();
PRIVATE void _initFSNodeDirectory(fs_node_t* node, char* name, u32int inode, u32int parent) ;

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index);
PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name);

PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

PRIVATE void fs_open(fs_node_t *node);
PRIVATE void fs_close(fs_node_t *node);

PRIVATE void appendDirectory(u32int inode, char* name, u32int newDirInode);

void fs_init() {
	diskManager_init(INODES);
	if (false && diskManager_validateHeader()) {
		fs_load();
	} else {
		fs_create();
	}
}

fs_node_t* fs_getRoot() {
	return &root;
}


PRIVATE void fs_create() {
	diskManager_writeHeader();				// Save header for the next time the system starts...
	int i;
	inodes = kmalloc(INODES * sizeof(iNode));
	for (i = 0; i < INODES; i++) {
		inodes[i].contents = NULL;
		inodes[i].length = 0;
	}
	// Initialize root directory
	int rootInode = diskManager_createiNode();
	_initFSNodeDirectory(&root, "~", rootInode, rootInode);
    //diskManager_updateiNodeContents(rootInode, inodes[rootInode].contents, inodes[rootInode].length);

	int devInode = diskManager_createiNode();
	fs_node_t *dev = kmalloc(sizeof(fs_node_t));
	_initFSNodeDirectory(dev, "dev", devInode, rootInode);
	appendDirectory(rootInode, dev->name, devInode);
    //diskManager_updateiNodeContents(devInode, inodes[devInode].contents, inodes[devInode].length);
}

PRIVATE void fs_load() {
	// TODO: hacer!
}

//==================================================================
//	callbacks - called when read/write/open/close are called.
//==================================================================

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index) {
	// 123 .'\0'
	// 247 ..'\0'
	// 260 pepe'\0'
	// 401 memos'\0'
	char* contents = inodes[node->inode].contents;
	u32int i = 0, offset = 0, len;
	while (i < index && offset < inodes[node->inode].length) {
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
		offset += len;								// skip fileName
		i++;
	}
	fs_node_t* fsnode = NULL;
	if (offset < inodes[node->inode].length) {
		fsnode = kmalloc(sizeof(fs_node_t));
		fsnode->flags = FS_DIRECTORY;
		memcpy(&fsnode->inode, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip inodenumber
		offset += sizeof(u32int);					// skip fileName
		strcpy(fsnode->name, contents + offset);
		if (inodes[fsnode->inode].contents == NULL) {
			//diskManager_readiNode(&inodes[fsnode->inode], fsnode->inode, MODE_ALL_CONTENTS);
		}
		return fsnode;
	}
	return NULL;
}

PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name) {
	return NULL;
}

PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	return 0;
}

PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	return 0;
}

PRIVATE void fs_open(fs_node_t *node) {
	/*currInodeindex++;
	currInodeindex %= INODES;
	diskManager_readiNode(node, currInodeindex, MODE_ALL_CONTENTS);
	inodes[currInodeindex] = node;*/
}

PRIVATE void fs_close(fs_node_t *node) {

}

/*
 * node = estructura a inicializar
 * name = nombre del directorio
 * inode = numero de inodo del nuevo directorio
 * parent = numero de inido de la carpeta padre
 */
PRIVATE void _initFSNodeDirectory(fs_node_t* node, char* name, u32int inode, u32int parent) {
    strcpy(node->name, name);
    node->inode = inode;
    node->flags = FS_DIRECTORY;
    node->mask = 0;
    node->uid = 0;
    node->gid = 0;
    node->read = 0;
    node->write = 0;
    node->open = 0;
    node->close = 0;
    node->readdir = fs_readdir;
    node->finddir = fs_finddir;
    node->ptr = 0;
    node->impl = 0;

    inodes[inode].length = 0;

    appendDirectory(inode, ".", inode);			// link to self
    appendDirectory(inode, "..", parent);		// link to parent
}

/*
 * inode = numero de inodo al cual se quiere agregar el directorio
 * name = nombre del nuevo directorio
 * newDirInode = numero de inido de nuva carpeta
 */
// FIXME: There should be a call to realloc! (need to be implemented)
PRIVATE void appendDirectory(u32int inode, char* name, u32int newDirInode) {
	iNode* node = &inodes[inode];
	int nameLen = strlen(name) + 1;
	int newLength = node->length + 2 * sizeof(u32int) + nameLen;

	char* newContents = kmalloc(newLength);
	if (node->contents != NULL) {
		// When adding a directory for the first time, contents have NULL value
		memcpy(newContents, node->contents, node->length);
		kfree(node->contents);
	}
	node->contents = newContents;
	newContents += node->length;
	node->length = newLength;

	memcpy(newContents, &inode, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, &nameLen, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, name, nameLen);
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

PRIVATE void findHole(FilePage* page, int size);


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



