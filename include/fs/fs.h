#ifndef ATADISK_H_
#define ATADISK_H_

#include <defs.h>
#include <fs/fs_API.h>
#include <fs/diskManager.h>

#define INODES					50

/*
 * inicializa el file system
 */
void fs_init();

/*
 * guarda en fsNode, todos los campos con los valores asociados a inodeNumber.
 */
void fs_getFsNode(fs_node_t* fsNode, u32int inodeNumber);

/*
 * retirna en fsNode todos los valores asociados al nodo root (/). 
 * Idem que llamar fs_getFsNode(fsNode, 0)
 */
void fs_getRoot(fs_node_t* fsNode);

/*
 * Crea una nuevo file en el directorio parentiNode, con nombre name y tipo type.
 */
u32int fs_createFile(u32int parentiNode, char* name, u32int type);

/*
 * Setea el access mode, sólamente admite cambios de permisos (no de type).
 */
PUBLIC void fs_setFileMode(u32int inode, int mode);

/*
 * Setea el uid del inodo.
 */
PUBLIC void fs_setFileUid(u32int inode, int uid);

/*
 * Setea el gid del inodo
 */
PUBLIC void fs_setFileGid(u32int inode, int gid);

#endif
