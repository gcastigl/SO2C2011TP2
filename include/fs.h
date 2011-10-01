#ifndef FS_H
#define FS_H

#include <defs.h>
#include <lib/kheap.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <driver/ata_disk.h>

#define MAX_FILENAME_LENGTH		64
#define MAX_FILES_PER_FOLDER	64
#define MAX_FOLDERS_PER_FOLDER	64

typedef struct {
	char name[MAX_FILENAME_LENGTH];
	u32int sector;
	u32int offset;
	char* contents;
	int length;
} iNode;

typedef struct Directory_t {
	char name[MAX_FILENAME_LENGTH];
	iNode* files[MAX_FILES_PER_FOLDER];
	u32int filesCount;
	struct Directory_t* parent;
	struct Directory_t* subDirs[MAX_FOLDERS_PER_FOLDER];
	u32int subDirsCount;
} Directory_t;

void fs_init();

void fs_createFolder(Directory_t* parent, char* name);

Directory_t* fs_getRootDirectory();

/*
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

struct fs_node;

// These typedefs define the type of callbacks - called when read/write/open/close
// are called.
typedef u32int (*read_type_t)(struct fs_node*,u32int,u32int,u8int*);
typedef u32int (*write_type_t)(struct fs_node*,u32int,u32int,u8int*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,u32int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

typedef struct fs_node
{
    char name[128];     // The filename.
    u32int mask;        // The permissions mask.
    u32int uid;         // The owning user.
    u32int gid;         // The owning group.
    u32int flags;       // Includes the node type. See #defines above.
    u32int inode;       // This is device-specific - provides a way for a filesystem to identify files.
    u32int length;      // Size of the file, in bytes.
    u32int impl;        // An implementation-defined number.
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr; // Used by mountpoints and symlinks.
} fs_node_t;

struct dirent
{
    char name[128]; // Filename.
    u32int ino;     // Inode number. Required by POSIX.
};

extern fs_node_t *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// , not file nodes.
u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

void open_fs(fs_node_t *node, u8int read, u8int write);

void close_fs(fs_node_t *node);

struct dirent *readdir_fs(fs_node_t *node, u32int index);

fs_node_t *finddir_fs(fs_node_t *node, char *name);
*/
#endif
