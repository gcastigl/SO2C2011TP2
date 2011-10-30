#include <fs/fs.h>
#include <session.h>

#define MAX(x,y)	(((x) > (y)) ? (x) : (y))

PRIVATE iNode inodes[INODES];				// List of file nodes.
PRIVATE u32int nextToFree = 0;

PRIVATE void fs_create();
PRIVATE void fs_load();

PRIVATE void _loadDirectory(int inodeNumber);

PRIVATE void _appendFile(u32int dirInodeNumber, u32int fileInodeNumber, char* name);

PRIVATE void _initInode(u32int inodeNumber, char* name, u32int flags);
PRIVATE void _initInode_dir(u32int inodeNumber, char* name, u32int parent);

// callbacks function declarations

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index);
PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name);

PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

PRIVATE void fs_open(fs_node_t *node);
PRIVATE void fs_close(fs_node_t *node);


PRIVATE int _indexOf(u32int inode);
PRIVATE int _loadInode(u32int inode);

void fs_init() {
	diskManager_init();
	diskCache_init();
	int i;
	for (i = 0; i < INODES; i++) {
		inodes[i].inodeId = -1;
		inodes[i].length = 0;
	}
	
	if (diskManager_validateHeader()) {
		fs_load();
	} else {
		fs_create();
	}
}

void fs_getRoot(fs_node_t* fsNode) {
	fs_getFsNode(fsNode, 0);
}

void fs_getFsNode(fs_node_t* fsNode, u32int inodeNumber) {
	// log(L_DEBUG, "loading %d node from memory", inodeNumber);
	int index =  _indexOf(inodeNumber);
	if (index == -1) {
		index = _loadInode(inodeNumber);
	}
	
	iNode* inode = &inodes[index];
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
	if ((inode->mask&FS_DIRECTORY) == FS_DIRECTORY) {
		fsNode->finddir = fs_finddir;
		fsNode->readdir = fs_readdir;
	} else {
		fsNode->finddir = NULL;
		fsNode->readdir = NULL;
	}
}

PRIVATE int _indexOf(u32int inode) {
	int i;
	for(i = 0; i < INODES; i++) {
		if (inodes[i].inodeId == inode) {
			//	log(L_DEBUG, "inode %d was found at position: %d", inode, i);
			return i;
		}
	}
	return -1;
}

PRIVATE int _loadInode(u32int inode) {
	errno = 0;
	//	log(L_DEBUG, "loading inode %d from memory", inode);
	diskManager_readInode(&inodes[nextToFree], inode);
	if (errno != 0) {
		log(L_ERROR, "error loading fsNode, errno is now: %d", errno);
		return -1;
	}
	int index = nextToFree++;
	nextToFree %= INODES;
	return index;
}

PRIVATE void fs_create() {
	diskManager_writeHeader(INODES);				// Save header for the next time the system starts...
	// Initialize root directory
	int rootInode = diskManager_nextInode();
	_initInode_dir(rootInode, "/", rootInode);

	int devInode = diskManager_nextInode();
	_initInode_dir(devInode, "dev", rootInode);

	int homeInode = diskManager_nextInode();
	_initInode_dir(homeInode, "home", rootInode);

	int rootHomeInode = diskManager_nextInode();
	_initInode_dir(rootHomeInode, "root", rootInode);

	int etcInode = diskManager_nextInode();
	_initInode_dir(etcInode, "etc", rootInode);

	// Add dev as sub-directory of root
	_appendFile(rootInode, devInode, NULL);
	_appendFile(rootInode, homeInode, NULL);
	_appendFile(rootInode, rootHomeInode, NULL);
	_appendFile(rootInode, etcInode, NULL);
}

PRIVATE void fs_load() {
	log(L_DEBUG, "loding OS from root...");
	_loadDirectory(0);			// Initialize root
}

PRIVATE void _loadDirectory(int inodeNumber) {
	int i = 0;
	fs_node_t curr;
	fs_getFsNode(&curr, inodeNumber);
	while(fs_readdir(&curr, i++) != NULL)
		;
}

u32int fs_createFile(u32int parentiNode, char* name, u32int type) {
	fs_node_t node;
	fs_getFsNode(&node, parentiNode);
	if (fs_finddir(&node, name) != NULL) {
		return E_FILE_EXISTS;
	}
	int inode = diskManager_nextInode();
	if (type == FS_DIRECTORY) {
		_initInode_dir(inode, name, parentiNode);
	} else {
		_initInode(inode, name, type | DEF_PERM);
	}
	_appendFile(parentiNode, inode, NULL);
	return inode;
}

PRIVATE void _initInode(u32int inodeNumber, char* name, u32int mask) {
	iNode* inode = &inodes[inodeNumber];
    inode->mask = mask;
    inode->flags = 0;
    inode->uid = session_getEuid();
    inode->gid = session_getEgid();
    inode->impl = 0;
    inode->length = 0;
    inode->inodeId = inodeNumber;
    strcpy(inode->name, name);
    diskManager_createInode(inode, inodeNumber, name);
}

PRIVATE void _initInode_dir(u32int inodeNumber, char* name, u32int parent) {
	_initInode(inodeNumber, name, FS_DIRECTORY | DEF_PERM);
	_appendFile(inodeNumber, inodeNumber, ".");	// link to self
	_appendFile(inodeNumber, parent, "..");		// link to parent
}

/*
 * If name == NULL, then the new file is created with the name specified by fileInodeNumber
 */
// FIXME: There should be a call to realloc! (need to be implemented)
PRIVATE void _appendFile(u32int dirInodeNumber, u32int fileInodeNumber, char* name) {
	char fileName[MAX_NAME_LENGTH];
	if (name == NULL) {
		diskManager_getFileName(fileInodeNumber, fileName);
	} else {
		strcpy(fileName, name);
	}
	if ((inodes[dirInodeNumber].mask&FS_DIRECTORY) != FS_DIRECTORY) {
		log(L_ERROR, "Trying to add file %s to a non dir!\n\n", name);
		errno = E_INVALID_ARG;
		return;
	}
	int contentsLength = diskManager_size(dirInodeNumber);
	// log(L_DEBUG, "_appendFile: int contLen = %d, appending %s", contentsLength, name);
	int nameLen = strlen(fileName) + 1;
	char content[contentsLength + 2 * sizeof(u32int) + nameLen];
	diskManager_readContents(dirInodeNumber, content, contentsLength, 0);

	int offset = contentsLength;

	memcpy(content + offset, &fileInodeNumber, sizeof(u32int));	offset += sizeof(u32int);
	memcpy(content + offset, &nameLen, sizeof(u32int));			offset += sizeof(u32int);
	memcpy(content + offset, fileName, nameLen); 				offset += nameLen;
	inodes[dirInodeNumber].length = offset;
		// log(L_DEBUG, "_appendFile: final contLen = %d, appending %s", offset, name);
	diskManager_writeContents(dirInodeNumber, content, offset, 0);
}



//==================================================================
//	callbacks - called when read/write/open/close are called.
//==================================================================

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index) {
	int length = diskManager_size(node->inode);
	char contents[length];
	diskManager_readContents(node->inode, contents, length, 0);
	u32int i = 0, offset = 0, len;

	// + 2 = skip "." and ".."
	while (i < index + 2 && offset < length) {
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
			// log(L_DEBUG, "file: %s - %d\n", contents + offset, len);
		offset += len;								// skip fileName
		i++;
	}
	fs_node_t* fsNode = NULL;
	if (offset < length) {
		int inodeNumber;
		memcpy(&inodeNumber, contents + offset, sizeof(u32int));
		// log(L_DEBUG, "directory %d... inode: %d\n", i, inodeNumber);
		fsNode = kmalloc(sizeof(fs_node_t));
		fs_getFsNode(fsNode, inodeNumber);
	}
	return fsNode;
}

PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name) {
	int length = diskManager_size(node->inode);
	char contents[length];
	diskManager_readContents(node->inode, contents, length, 0);
	u32int offset = 0, len, inodeNumber;
	while (offset < inodes[node->inode].length) {
		memcpy(&inodeNumber, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip inodeNumber
		memcpy(&len, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip strlen
		if (strcmp(name, contents + offset) == 0) {
			fs_node_t* fsnode = kmalloc(sizeof(fs_node_t));
			fs_getFsNode(fsnode, inodeNumber);
			return fsnode;
		}
		offset += len;								// skip fileName
	}
	return NULL;
}


u32int fs_size(fs_node_t *node) {
	return diskManager_size(node->inode);
}


PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	int index =  _indexOf(node->inode);
	if (index == -1) {
		index = _loadInode(node->inode);
	}
    iNode header = inodes[node->inode];
    //	log(L_DEBUG, "%d - %s => reading %d bytes from offset %d (inode length: %d)", node->inode, node->name, size, offset, header.length);
    if (offset > header.length) {
        return 0;
    }
    if (offset + size > header.length) {
        size = header.length - offset;
    }
    diskManager_readContents(node->inode, (char*) buffer, size, offset);
    return size;
}

PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
    //	log(L_DEBUG, "%d - %s => writing %d bytes from offset %d", node->inode, node->name, size, offset);
    int index = _indexOf(node->inode);
	if (index == -1) {
		index = _loadInode(node->inode);
	}
	inodes[index].length = MAX(inodes[index].length, offset + size);
    diskManager_writeContents(node->inode, (char*) buffer, size, offset);
    return size;
}


PRIVATE void fs_open(fs_node_t *node) {

}

PRIVATE void fs_close(fs_node_t *node) {

}



