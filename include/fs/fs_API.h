#ifndef FS_H
#define FS_H

#include <defs.h>
#include <util/logger.h>

#define MAX_NAME_SIZE	128



//#define FS_FILE        0x01
//#define FS_DIRECTORY   0x02
//#define FS_CHARDEVICE  0x03
//#define FS_BLOCKDEVICE 0x04
//#define FS_PIPE        0x05
//#define FS_SYMLINK     0x06
//#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?
#define FS_TYPE        0xF000
#define FS_PIPE        0x1000
#define FS_CHARDEVICE  0x2000
#define FS_DIRECTORY   0x4000
#define FS_BLOCKDEVICE 0x6000
#define FS_FILE        0x8000
#define FS_SYMLINK     0xA000
#define FS_SOCKET      0xC000

#define DEF_PERM	   0x0644

#define R_BIT	0x0004	/* Rwx protection bit */
#define W_BIT	0x0002	/* rWx protection bit */
#define X_BIT	0x0001	/* rwX protection bit */

#define S_IRWXU 0x0700 //User RWX
#define S_IRUSR 0x0400 //User R
#define S_IWUSR 0x0200 //User W
#define S_IXUSR 0x0100 //User X
#define S_IRWXG 0x0070 //Group RWX
#define S_IRGRP 0x0040 //Group R
#define S_IWGRP 0x0020 //Group W
#define S_IXGRP 0x0010 //Group X
#define S_IRWXO 0x0007 //Others RWX
#define S_IROTH 0x0004 //Others R
#define S_IWOTH 0x0002 //Others W
#define S_IXOTH 0x0001 //Others X

struct fs_node;

// These typedefs define the type of callbacks - called when read/write/open/close
// are called.
typedef u32int (*read_type_t)(struct fs_node*,u32int,u32int,u8int*);
typedef u32int (*write_type_t)(struct fs_node*,u32int,u32int,u8int*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct fs_node * (*readdir_type_t)(struct fs_node*,u32int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

typedef struct fs_node {
    char name[MAX_NAME_SIZE];     	// The filename.
    u32int mask;        			// The permissions mask.
    u32int uid;        				// The owning user.
    u32int gid;         			// The owning group.
    u32int flags;       			// Includes the node type. See #defines above.
    u32int inode;       			// This is device-specific - provides a way for a filesystem to identify files.
    u32int impl;        			// An implementation-defined number.
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr; 			// Used by mountpoints and symlinks.
} fs_node_t;

struct dirent {
    char name[MAX_NAME_SIZE]; 	// Filename.
    u32int ino;     			// Inode number. Required by POSIX.
};

extern fs_node_t *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// , not file nodes.
u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

void open_fs(fs_node_t *node, u8int read, u8int write);

void close_fs(fs_node_t *node);

fs_node_t *readdir_fs(fs_node_t *node, u32int index);

fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif
