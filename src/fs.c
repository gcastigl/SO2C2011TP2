#include <fs.h>

#define MAX_FILES 	1024
#define FS_HEADER	"GAT_OS_FS"
static iNode iNodes[MAX_FILES];
static Directory_t root;

void fs_create();
void fs_load();
boolean validate_header();
void write_header();

void initEmptyDirectory(Directory_t* dir, char* name);

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
	for(i = 0; i < MAX_FILES; i++) {
		iNodes[i].contents = NULL;
	}
	// create root
	printf("Creating root directory...\n");
	//root = (Directory_t*) kmalloc(sizeof(Directory_t));
	initEmptyDirectory(&root, "~");
	// create /dev
	printf("Creating /dev directory...\n");
	fs_createSubDirectory(&root, "dev");
}

int fs_createSubDirectory(Directory_t* dir, char* name) {
	if(fs_directoryExist(dir, name)) {
		return FS_DIR_EXISTS;
	} else if (dir->subDirsCount == MAX_FOLDERS_PER_FOLDER) {
		return FS_DIR_FULL;
	}
	Directory_t* newFolder = (Directory_t*) kmalloc(sizeof(Directory_t));
	initEmptyDirectory(newFolder, name);
	dir->subDirs[dir->subDirsCount++] = newFolder;
	return 0;
}

void initEmptyDirectory(Directory_t* dir, char* name) {
	int i;
	strcpy(dir->name, name);
	dir->filesCount = 0;
	for(i = 0; i < MAX_FILES_PER_FOLDER; i++) {
		dir->files[i] = NULL;
	}
	dir->subDirsCount = 0;
	for(i = 0; i < MAX_FOLDERS_PER_FOLDER; i++) {
		dir->subDirs[i] = NULL;
	}
}

Directory_t* fs_getDirectory(Directory_t* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return dir->subDirs[i];
		}
	}
	return NULL;
}

Directory_t* fs_getRootDirectory() {
	return &root;
}

boolean fs_directoryExist(Directory_t* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return true;
		}
	}
	return false;
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

