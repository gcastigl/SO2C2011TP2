#include <fs/fs.h>

PRIVATE iNode *inodes;				// List of file nodes.

PRIVATE void fs_create();
PRIVATE void fs_load();

PRIVATE void _initInode(u32int inodeNumber, char* name, u32int flags);
PRIVATE void _initInode_dir(u32int inodeNumber, char* name, u32int parent);
PRIVATE void appendDirectory(u32int inode, char* name, u32int dirInode);

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index);
PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name);

PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

PRIVATE void fs_open(fs_node_t *node);
PRIVATE void fs_close(fs_node_t *node);


void fs_init() {
	diskManager_init(INODES);
	if (false && diskManager_validateHeader()) {
		fs_load();
	} else {
		fs_create();
	}
}

void fs_getRoot(fs_node_t* fsNode) {
	fs_getFsNode(fsNode, 0);
}

void fs_getFsNode(fs_node_t* fsNode, u32int inodeNumber) {
	iNode* inode = &inodes[inodeNumber];
	strcpy(fsNode->name, inode->name);
	fsNode->flags = inode->flags;
	fsNode->gid = inode->gid;
	fsNode->uid = inode->uid;
	fsNode->impl = inode->impl;
	fsNode->mask = inode->mask;
	fsNode->inode = inodeNumber;
	//POSSIBLE FIXME: this function assignments could be different(depending of the flags maybe?)
	fsNode->read = fs_read;
	fsNode->write = fs_write;
	fsNode->open = fs_open;
	fsNode->close = fs_close;
	if ((inode->flags&0x07) == FS_DIRECTORY) {
		fsNode->finddir = fs_finddir;
		fsNode->readdir = fs_readdir;
	} else {
		fsNode->finddir = NULL;
		fsNode->readdir = NULL;
	}
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
	_initInode_dir(rootInode, "~", rootInode);

	int devInode = diskManager_nextInode();
	_initInode_dir(devInode, "dev", rootInode);

	// add dev as sub-directory of root
	appendDirectory(rootInode, "dev", devInode);
}

PRIVATE void fs_load() {
	// TODO: hacer!
}

//==================================================================
//	callbacks - called when read/write/open/close are called.
//==================================================================

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index) {
	char* contents = inodes[node->inode].contents;
	u32int i = 0, offset = 0, len;
	// + 2 = skip . and ..
	while (i < index + 2 && offset < inodes[node->inode].length) {
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
		offset += len;								// skip fileName
		i++;
	}
	if (offset < inodes[node->inode].length) {
		int inodeNumber;
		memcpy(&inodeNumber, contents + offset, sizeof(u32int));
		fs_node_t* fsNode = kmalloc(sizeof(fs_node_t));
		fs_getFsNode(fsNode, inodeNumber);
		return fsNode;
	}
	return NULL;
}

PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name) {
	char* contents = inodes[node->inode].contents;
	u32int offset = 0, len;
	while (offset < inodes[node->inode].length) {
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
		if (strcmp(name, contents + offset) == 0) {
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
}

PRIVATE void fs_close(fs_node_t *node) {

}

PRIVATE void _initInode(u32int inodeNumber, char* name, u32int flags) {
	iNode* inode = &inodes[inodeNumber];
	strcpy(inode->name, name);
    inode->flags = flags;
    inode->mask = 0;
    inode->uid = 0;
    inode->gid = 0;
    inode->impl = 0;
    inode->length = 0;
    inode->contents = NULL;
    inode->deviceId = inodeNumber;
    diskManager_updateiNodeContents(inode, inodeNumber);
}

PRIVATE void _initInode_dir(u32int inodeNumber, char* name, u32int parent) {
	_initInode(inodeNumber, name, FS_DIRECTORY);
	appendDirectory(inodeNumber, ".", inodeNumber);			// link to self
	appendDirectory(inodeNumber, "..", parent);				// link to parent
	diskManager_updateiNodeContents(&inodes[inodeNumber], inodeNumber);
}

// FIXME: There should be a call to realloc! (need to be implemented)
PRIVATE void appendDirectory(u32int inodeNumber, char* name, u32int dirInode) {
	iNode* inode = &inodes[inodeNumber];
	if ((inode->flags&0x07) != FS_DIRECTORY) {
		errno = E_INVALID_ARG;
		return;
	}
	int nameLen = strlen(name) + 1;
	int newLength = inode->length + 2 * sizeof(u32int) + nameLen;

	char* newContents = kmalloc(newLength);
	if (inode->contents != NULL) {
		// When adding a directory for the first time, contents have NULL value
		memcpy(newContents, inode->contents, inode->length);
		kfree(inode->contents);
	}
	inode->contents = newContents;
	newContents += inode->length;
	inode->length = newLength;

	memcpy(newContents, &dirInode, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, &nameLen, sizeof(u32int));	newContents += sizeof(u32int);
	memcpy(newContents, name, nameLen);
}




