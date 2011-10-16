#ifndef ATADISK_H_
#define ATADISK_H_

#include <defs.h>
#include <fs/fs_API.h>
#include <fs/diskManager.h>
#include <driver/video.h>

#define INODES					50

void fs_init();

void fs_getRoot(fs_node_t* fsNode);

void fs_getFsNode(fs_node_t* fsNode, u32int inodeNumber);

#endif
