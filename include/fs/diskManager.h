#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <fs/diskCache.h>
#include <driver/ata_disk.h>
#include <lib/string.h>

#define MAGIC_NUMBER					123456
#define FILE_BLOCK_OVERHEAD_SIZE_BYTES	(sizeof(DiskPage) + sizeof(FileHeader))

#define DISK_BLOCK_SIZE_BYTES			256

// Available strategies for disk accesing
#define S_DIRECT_ACCESS		0
#define S_LRU_CACHE			1

typedef struct {
	// fields required by posix, this is still missing some fields
	u32int sector;
	u32int offset;
    u32int inodeId;        			// Identifies the structure
    u32int uid;        				// The owning user.
    u32int gid;         			// The owning group.
    u32int flags;       			// Includes the node type.
    u32int length;      			// Size of the file, in bytes.
    u32int impl;        			// An implementation-defined number.
    u32int mask;        			// The permissions mask.
    char name[MAX_NAME_LENGTH];		// File name
} iNode;

typedef struct {
	u32int magic;
	u32int maxNodes;
} FSHeader;

typedef struct {
	u32int magic;
	u32int disk;
	u32int nextSector;
	u32int nextOffset;
	u32int totalLength;
	u32int usedBytes;
	boolean hasNextPage;
} DiskPage;

typedef struct {
	DiskPage data;
	u32int blocks;
	u32int usedBytes;
} iNodeDisk;

typedef struct {
	u32int magic;
	char name[MAX_NAME_LENGTH];
    u32int mask;        			// The permissions mask.
    u32int uid;        				// The owning user.
    u32int gid;         			// The owning group.
    u32int flags;       			// Includes the node type. See #defines above.
    u32int impl;        			// An implementation-defined number.
} FileHeader;

typedef void (*disk_access_t)(int disk, void* msg, int bytes, unsigned short sector, int offset);

typedef struct {
	disk_access_t write;
	disk_access_t read;
} disk_strategy;

/*
 + Inicializa al diskManager
 */
void diskManager_init(u32int strategyType);

/*
 + Intenta leer del sector 0 del disco, una estructura de tipo FSHeader y valida su magic number. En casoo de macheo, retorna true.
 */
boolean diskManager_validateHeader();

/*
 + Escribe en el sector 0 del disco duro, una estructura valida de FSHeader, de esta manera, la proxima ve que el SO inicialize, se sabra que ya existen datos validos.
 */
void diskManager_writeHeader();

/*
Retorna un int representando un numero de inode libre para ser utilizado para crear un nuevo archivo.
*/
int diskManager_nextInode();

/*
 + Guarda en disco ek iNode resivido en el primer parametros con nombre name como numero de inodo iNodeNumber.
 */
void diskManager_createInode(iNode* inode, u32int inodeNumber, char* name);

/*
 * Elimina del disco la informacion relacionada con el inode numero inode
 */
void diskManager_delete(u32int inode);

/*
 + Carga del disco el inodo que este guardado como inodeNumber y lo guarda en inode. En caso de error, se setea errno con el valor inidicando el tipo de error que ocurrio.
 */
void diskManager_readInode(iNode *inode, u32int inodeNumber);

/*
 * Escribe length bytes de contents con el offset indicado, en el contenido de inodeNumber. En caso que offset + length sea mayor al largo del contenido del nodo ya exitente, simplemente se exitende la memoria y el nuevo largo es ahora = offset + length.
 */
int diskManager_writeContents(u32int inodeNumber, char* contents, u32int length, u32int offset);

/*
 * Lee length bytes de contents con el offset indicado, en el contenido de inodeNumber. En caso que offset + length sea mayor al largo del contenido del nodo ya exitente, simplemente se lee hasta el final del archivo.
 */
int diskManager_readContents(u32int inodeNumber, char* contents, u32int length, u32int offset);

/*
 * retorna en name, el nombre asociado al inodo "inode", name tiene que contener como minimo MIN_NAME_LENGTH bytes disponibles
*/
void diskManager_getFileName(u32int inode, char* name);

/*
 * renombra el inodo inode con el nombre dado en el segundo parametro. Se guardan los primeros MIN_NAME_LENGTH bytes.
 */
void diskManager_setFileName(u32int inode, char* name);

/*
 * Retorna un entero indianco la cnatidad de bytesque ocupa el inodo inodenumber.
 */
u32int diskManager_size(u32int inodeNumber);

/*
 * Setea el modo del file header
 */
boolean diskManager_setFileMode(u32int inode, int newMode);

/*
 * Setea el uid del file header
 */
boolean diskManager_setFileUid(u32int inode, int uid);

/*
 * Setea el gid del file header
 */
boolean diskManager_setFileGid(u32int inode, int gid);

#endif
