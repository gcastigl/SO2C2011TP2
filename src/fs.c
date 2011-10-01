#include <fs.h>

#define FS_HEADER	"GAT_OS_FS"
static iNode iNodes[MAX_INODES];

void fs_create();
void fs_load();
boolean validate_header();
void write_header();


void fs_init() {
	printf("Initializing the file system...\n");
	if (validate_header()) {
		//fs_load();
		fs_create();
	} else {
		fs_create();
	}
	printf("OK!");
}

boolean validate_header() {
	int len = strlen(FS_HEADER);
	char header[len];
	read(ATA0, header, len, 0, 0);
	return strcmp(header, FS_HEADER) == 0;
}

void write_header() {
	write(ATA0, FS_HEADER, strlen(FS_HEADER), 0, 0);
}

void fs_load() {
	printf("loading a file system!\n");
}

void fs_create() {
	printf("Creating a new file system!\n");

	write_header();
	int i;
	for(i = 0; i < MAX_INODES; i++) {
		iNodes[i].contents = NULL;
	}
	// create root
	printf("Creating root directory...\n");
	directory_initialize();
	// create /dev
	printf("Creating /dev directory...\n");
	directory_create(directory_getRoot(), "dev");
}


/*
fs_node_t *fs_root = 0; // The root of the filesystem.

u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer)
{
    // Has the node got a read callback?
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer)
{
    // Has the node got a write callback?
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void open_fs(fs_node_t *node, u8int read, u8int write)
{
    // Has the node got an open callback?
    if (node->open != 0)
        return node->open(node);
}

void close_fs(fs_node_t *node)
{
    // Has the node got a close callback?
    if (node->close != 0)
        return node->close(node);
}

struct dirent *readdir_fs(fs_node_t *node, u32int index)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->readdir != 0 )
        return node->readdir(node, index);
    else
        return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->finddir != 0 )
        return node->finddir(node, name);
    else
        return 0;
}*/

