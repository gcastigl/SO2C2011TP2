#ifndef ATADISK_H_
#define ATADISK_H_

#include <defs.h>
#include <fs/fs_API.h>
#include <fs/diskManager.h>
#include <driver/ata_disk.h>
#include <driver/video.h>

#define INODES					50

void fs_init();

fs_node_t* fs_getRoot();

#endif
