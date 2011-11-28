#include <fs/fs_API.h>
#include <util/logger.h>

u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
    // Has the node got a read callback?
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else {
    	log(L_ERROR, "%s does not have a callback for read_fs", node->name);
        return 0;
    }
}

u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
    // Has the node got a write callback?
    if (node->write != 0) {
        return node->write(node, offset, size, buffer);
    } else {
    	log(L_ERROR, "%s does not have a callback for write_fs", node->name);
        return 0;
    }
}

void open_fs(fs_node_t *node) {
    // Has the node got an open callback?
    if (node->open != 0) {
        node->open(node);
    } else {
    	log(L_ERROR, "%s does not have a callback for open_fs", node->name);
    }
}

void close_fs(fs_node_t *node) {
    // Has the node got a close callback?
    if (node->close != 0) {
        node->close(node);
    } else {
    	log(L_ERROR, "%s does not have a callback for close_fs", node->name);
    }
}

fs_node_t *readdir_fs(fs_node_t *node, u32int index) {
    // Is the node a directory, and does it have a callback?
    if (FILE_TYPE(node->mask) == FS_DIRECTORY && node->readdir != 0) {
        return node->readdir(node, index);
    } else {
    	log(L_ERROR, "%s does not have a callback for readdir_fs", node->name);
        return 0;
    }
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    // Is the node a directory, and does it have a callback?
    if (FILE_TYPE(node->mask) == FS_DIRECTORY && node->finddir != 0)
        return node->finddir(node, name);
    else {
    	log(L_ERROR, "%s does not have a callback for finddir_fs", node->name);
        return 0;
    }
}

u32int removedir_fs(fs_node_t *node, u32int inode) {
	if (FILE_TYPE(node->mask) == FS_DIRECTORY && node->removedir != NULL)
		return node->removedir(node, inode);
	else {
		log(L_ERROR, "%s does not have a callback for finddir_fs", node->name);
		return -1;
	}
}

u32int createdir_fs(fs_node_t* node, char* name, u32int type) {
	if (FILE_TYPE(node->mask) == FS_DIRECTORY && node->createdir != NULL)
		return node->createdir(node, name, type);
	else {
		log(L_ERROR, "%s does not have a callback for createdir_fs", node->name);
		return -1;
	}
}

u32int size_fs(fs_node_t* node) {
	if (node->size != NULL)
		return node->size(node);
	else {
		log(L_ERROR, "%s does not have a callback for size_fs", node->name);
		return -1;
	}
}
