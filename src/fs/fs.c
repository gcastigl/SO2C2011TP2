#include <fs/fs.h>
#include <session.h>

#define MAX(x,y)	(((x) > (y)) ? (x) : (y))

PRIVATE iNode inodes[INODES];				// List of file nodes.
PRIVATE u32int nextToFree = 0;

PRIVATE void fs_create();

// callbacks function declarations
PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index);
PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name);
PRIVATE u32int fs_createdir(fs_node_t* node, char* name, u32int type);
PRIVATE u32int fs_removedir(fs_node_t *node, u32int inode);
PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
PRIVATE u32int fs_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
PRIVATE void fs_open(fs_node_t *node);
PRIVATE void fs_close(fs_node_t *node);
PRIVATE u32int fs_size(fs_node_t *node);

// Auxiliary funcions
PRIVATE void _appendFile(u32int dirInodeNumber, u32int fileInodeNumber, char* name);
PRIVATE void _initInode(u32int inodeNumber, char* name, u32int flags);
PRIVATE void _initInode_dir(u32int inodeNumber, char* name, u32int parent);
PRIVATE int _indexOf(u32int inode);
PRIVATE int _loadInode(u32int inode);

void fs_init() {
	diskManager_init(S_LRU_CACHE);
	diskCache_init();
	int i;
	for (i = 0; i < INODES; i++) {
		inodes[i].inodeId = -1;
		inodes[i].length = 0;
	}
	if (!diskManager_validateHeader()) {
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
		log(L_ERROR, "could not load inode: %d", inodeNumber);
		return;
	}
	iNode* inode = &inodes[index];
	memcpy(fsNode->name, inode->name, strlen(inode->name) + 1);
	fsNode->flags = inode->flags;
	fsNode->gid = inode->gid;
	fsNode->uid = inode->uid;
	fsNode->impl = inode->impl;
	fsNode->mask = inode->mask;
	fsNode->inode = inodeNumber;
	if (FILE_TYPE(inode->mask) == FS_PIPE) {
		fsNode->read = fifo_read;
		fsNode->write = fifo_write;
	} else {
		fsNode->read = fs_read;
		fsNode->write = fs_write;
	}
	fsNode->open = fs_open;
	fsNode->close = fs_close;
	fsNode->size = fs_size;
	if (FILE_TYPE(inode->mask) == FS_DIRECTORY) {
		fsNode->finddir = fs_finddir;
		fsNode->readdir = fs_readdir;
		fsNode->removedir = fs_removedir;
		fsNode->createdir = fs_createdir;
	} else {
		fsNode->finddir = NULL;
		fsNode->readdir = NULL;
		fsNode->removedir = NULL;
		fsNode->createdir = NULL;
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
	return _loadInode(inode);
}

PRIVATE int _loadInode(u32int inode) {
	//	log(L_DEBUG, "loading inode %d from memory", inode);
	errno = 0;
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
	// Save header for the next time the system starts
	diskManager_writeHeader();
	// Initialize root directory
	int rootInode = diskManager_nextInode();
	_initInode_dir(rootInode, "/", rootInode);
	fs_setFileMode(rootInode, 0x777);
	// Create initial directories
	fs_node_t root;
	fs_getRoot(&root);
	fs_createdir(&root, "dev", FS_DIRECTORY);
	fs_createdir(&root, "home", FS_DIRECTORY);
	fs_createdir(&root, "etc", FS_DIRECTORY);
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
	if (FILE_TYPE(inodes[dirInodeNumber].mask) != FS_DIRECTORY) {
		log(L_ERROR, "Trying to add file %s to a non dir!\n\n", name);
		errno = E_INVALID_ARG;
		return;
	}
	int contentsLength = diskManager_size(dirInodeNumber);
	char content[contentsLength + sizeof(u32int) + MAX_NAME_LENGTH];
	diskManager_readContents(dirInodeNumber, content, contentsLength, 0);
	int offset = 0;
	while (offset < contentsLength) {		// Try to use a logically deleted file
		if ((int) *(content + offset) == -1) {
			memcpy(content + offset, &fileInodeNumber, sizeof(u32int));
			memcpy(content + offset + sizeof(u32int), fileName, MAX_NAME_LENGTH);
			diskManager_writeContents(dirInodeNumber, content, contentsLength, 0);
			return;
		}
		offset += sizeof(u32int) + MAX_NAME_LENGTH;
	}
	offset = contentsLength;
	memcpy(content + offset, &fileInodeNumber, sizeof(u32int));	offset += sizeof(u32int);
	memcpy(content + offset, fileName, MAX_NAME_LENGTH); 		offset += MAX_NAME_LENGTH;
	inodes[dirInodeNumber].length = offset;
	// log(L_DEBUG, "_appendFile: final contLen = %d, appending %s", offset, name);
	// FIXME: write last part instead of the whole contents again
	diskManager_writeContents(dirInodeNumber, content, offset, 0);
}



//==================================================================
//	callbacks - called when read/write/open/close are called.
//==================================================================

PRIVATE fs_node_t *fs_readdir(fs_node_t *node, u32int index) {
	int length = diskManager_size(node->inode);
	char contents[length];
	diskManager_readContents(node->inode, contents, length, 0);
	u32int i, offset, inodeNumber = -1;

	i = 0;
	offset = 2 * (sizeof(u32int) + MAX_NAME_LENGTH); // skip "." and ".."
	while (i <= index && offset < length) {
		memcpy(&inodeNumber, contents + offset, sizeof(u32int));
		//	log(L_DEBUG, "file: %s - %d", contents + offset + sizeof(u32int), inodeNumber);
		offset += sizeof(u32int) + MAX_NAME_LENGTH;
		if (inodeNumber != -1) { // inodeNumber == -1 mens it was removed and the slot is free
			i++;
		}
	}
	fs_node_t* fsNode = NULL;
	//	log(L_DEBUG, "inode: %d, idnex: %d, i: %d", inodeNumber, index, i);
	if (i - 1 == index) {
		//	log(L_DEBUG, "directory %d... inode: %d\n", i, inodeNumber);
		fsNode = kmalloc(sizeof(fs_node_t));
		fs_getFsNode(fsNode, inodeNumber);
	}
	return fsNode;
}

PRIVATE fs_node_t *fs_finddir(fs_node_t *node, char *name) {
	int length = diskManager_size(node->inode);
	char contents[length];
	diskManager_readContents(node->inode, contents, length, 0);
	u32int offset = 0, inodeNumber;
	while (offset < length) {
		memcpy(&inodeNumber, contents + offset, sizeof(u32int));
		offset += sizeof(u32int);					// skip inodeNumber
		if (inodeNumber != -1 && strcmp(name, contents + offset) == 0) {
			fs_node_t* fsnode = kmalloc(sizeof(fs_node_t));
			fs_getFsNode(fsnode, inodeNumber);
			return fsnode;
		}
		offset += MAX_NAME_LENGTH;					// skip fileName
	}
	return NULL;
}

PRIVATE u32int fs_createdir(fs_node_t* node, char* name, u32int type) {
	if (!permission_file_hasAccess(node, W_BIT)) {
	    errno = EACCES;
	    return -1;
	}
	if (fs_finddir(node, name) != NULL) {
		errno = E_FILE_EXISTS;
		return -1;
	}
	int inode = diskManager_nextInode();
	if (type == FS_DIRECTORY) {
		_initInode_dir(inode, name, node->inode);
	} else {
		_initInode(inode, name, type | DEF_PERM);
	}
	_appendFile(node->inode, inode, NULL);
	return inode;
}

PRIVATE u32int fs_removedir(fs_node_t* node, u32int inode) {
	int index = _indexOf(node->inode);
	if (index == -1) {
    	log(L_ERROR, "could not load inode: %d", node->inode);
    	return E_ACCESS;
	}
	// inodes[inode].length = diskManager_size(inode);
		log(L_DEBUG, "removing inode: %d, length: %d", inode, inodes[index].length);
	char contents[inodes[index].length];
	diskManager_readContents(node->inode, contents, inodes[index].length, 0);
	int offset = 0, readinode;
	while (offset < inodes[index].length) {
		memcpy(&readinode, contents + offset, sizeof(u32int));
		//	log(L_DEBUG, "delete: read inode %d - %s", readinode, contents + offset + sizeof(u32int));
		if (readinode == inode) {
			readinode = -1;
			memcpy(contents + offset, &readinode, sizeof(u32int));
				log(L_DEBUG, "%d was removed succesfully!", inode);
			diskManager_writeContents(node->inode, contents + offset, sizeof(u32int), offset);
			diskManager_delete(inode);
			return 0;
		}
		offset += sizeof(u32int) + MAX_NAME_LENGTH;
	}
	return E_FILE_NOT_EXISTS;
}

PRIVATE u32int fs_size(fs_node_t *node) {
	int index =  _indexOf(node->inode);
	if (index != -1) {
		return inodes[index].length;
	}
	return diskManager_size(node->inode);
}


PRIVATE u32int fs_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	int index =  _indexOf(node->inode);
	if (index == -1) {
		log(L_ERROR, "could not load inode: %d", node->inode);
		return 0;
	}
    iNode header = inodes[index];
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
		log(L_ERROR, "could not load inode: %d", node->inode);
		return 0;
	}
	inodes[index].length = MAX(inodes[index].length, offset + size);
    diskManager_writeContents(node->inode, (char*) buffer, size, offset);
    return size;
}


PRIVATE void fs_open(fs_node_t *node) {
	log(L_DEBUG, "fs_open...");
	PROCESS* p = getCurrentProcess();
	if (p == NULL) {
		log(L_ERROR, "current process is null!");
		return;
	}
	log(L_DEBUG, "opening file %s, process: %d", node->name, p);
	boolean fileOpened = false;
	for(int i = 0; i < MAX_FILES_PER_PROCESS && !fileOpened; i++) {
		if (p->fd_table[i].mask == 0) {
			log(L_DEBUG, "using index %d to open file: %s, mode: %d", node->name, node->flags);
			p->fd_table[i].mask = FILE_TYPE(node->mask);
			p->fd_table[i].mode = node->flags;
			p->fd_table[i].inode = node->inode;
			p->fd_table[i].count = 1;
			fileOpened = true;
		}
	}
	log(L_DEBUG, "node opened? => %d", fileOpened);
	if (!fileOpened) {
		errno = E_MAX_FD_REACHED;
	}
}

PRIVATE void fs_close(fs_node_t *node) {
	PROCESS* p = getCurrentProcess();
	for(int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
		if (p->fd_table[i].inode == node->inode) {
			p->fd_table[i].mask = 0;
		}
	}
}


PUBLIC void fs_setFileMode(u32int inode, int mode) {
    mode &= (S_IRWXU | S_IRWXG | S_IRWXO);
    int index = _indexOf(inode);
    if (index == -1) {
    	log(L_ERROR, "Unable to load inode: %d", inode);
        return;
    }
    int newMode = FILE_TYPE(inodes[index].mask) | mode;
    log(L_DEBUG, "changing inode %d to %x mode [old: %x]",
            inode,
            newMode,
            inodes[index].mask
    );
    if (diskManager_setFileMode(inode, newMode)) {
        inodes[index].mask = newMode;
    }
}

PUBLIC void fs_setFileUid(u32int inode, int uid) {
    int index = _indexOf(inode);
    if (index == -1) {
    	log(L_ERROR, "could not load inode: %d", inode);
    	return;
    }
    if (diskManager_setFileUid(inode, uid)) {
        inodes[index].uid = uid;
    }
}

PUBLIC void fs_setFileGid(u32int inode, int gid) {
    int index = _indexOf(inode);
    if (index == -1) {
    	log(L_ERROR, "could not load inode: %d", inode);
    	return;
    }
    if (diskManager_setFileGid(inode, gid)) {
        inodes[index].gid = gid;
    }
}


