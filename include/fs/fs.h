#ifndef ATADISK_H_
#define ATADISK_H_

#include <defs.h>
#include <fs/fs_API.h>
#include <fs/diskManager.h>

#define INODES					50

void fs_init();

void fs_getRoot(fs_node_t* fsNode);

void fs_getFsNode(fs_node_t* fsNode, u32int inodeNumber);

u32int fs_createFile(u32int parentiNode, char* name, u32int type);

PUBLIC void fs_setFileMode(u32int inode, int mode);
PUBLIC void fs_setFileUid(u32int inode, int uid);
PUBLIC void fs_setFileGid(u32int inode, int gid);

#endif
