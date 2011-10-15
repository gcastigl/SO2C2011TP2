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
	int rootInode = diskManager_nextInode();
	_initFSNodeDirectory(&root, "~", rootInode, rootInode);
    //diskManager_updateiNodeContents(rootInode, inodes[rootInode].contents, inodes[rootInode].length);

	int devInode = diskManager_nextInode();
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
		offset += sizeof(u32int);					// skip strlen
		strcpy(fsnode->name, contents + offset);
		if (inodes[fsnode->inode].contents == NULL) {
			//diskManager_readiNode(&inodes[fsnode->inode], fsnode->inode, MODE_ALL_CONTENTS);
		}
		return fsnode;
	}
	return NULL;
}

PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name) {
	printf("recieved nodes: %s\n", node->name);
	char* contents = inodes[node->inode].contents;
	u32int offset = 0, len;
	while (offset < inodes[node->inode].length) {
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
		if (strcmp(name, contents + offset) == 0) {
			printf("directory found!!\n");
			fs_node_t* fsnode = kmalloc(sizeof(fs_node_t));
			fsnode->flags = FS_DIRECTORY;
			memcpy(&fsnode->inode, contents + offset - 2 * sizeof(u32int), sizeof(u32int));
			strcpy(fsnode->name, contents + offset);
			if (inodes[fsnode->inode].contents == NULL) {
				//diskManager_readiNode(&inodes[fsnode->inode], fsnode->inode, MODE_ALL_CONTENTS);
			}
			return fsnode;
		}
		offset += len;								// skip fileName
	}
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

	memcpy(newContents, &newDirInode, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, &nameLen, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, name, nameLen);
}




