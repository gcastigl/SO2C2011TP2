#ifndef AT_WINI_H
#define AT_WINI_H

#include <asm/libasm.h>
#include <lib/stdio.h>

/* I/O Ports used by winchester disk controller. */
#define ATA0 0x1f0
#define ATA1 0x170

#define WIN_REG0       0x0
#define WIN_REG1       0x1 // Error
#define WIN_REG2       0x2
#define WIN_REG3       0x3
#define WIN_REG4       0x4
#define WIN_REG5       0x5
#define WIN_REG6       0x6
#define WIN_REG7       0x7 // Command|Status
#define WIN_REG8       0x3f6


/* Winchester disk controller command bytes. */
#define WIN_IDENTIFY	0xEC
#define MEDIA_STATUS	0xDA
#define READ_DMA		0xC8
#define WIN_RECALIBRATE	0x10	/* command for the drive to recalibrate */
#define WIN_READ        0x20	/* command for the drive to read */
#define WIN_WRITE       0x30	/* command for the drive to write */
#define WIN_SPECIFY     0x91	/* command for the controller to accept params */

#define LBA_READ        WIN_READ
#define LBA_WRITE       WIN_WRITE

/* Parameters for the disk drive. */
#define SECTOR_SIZE      512	/* physical sector size in bytes */

/* Error codes */
#define ERR		  -1	/* general error */

/* Miscellaneous. */
#define MAX_ERRORS         4	/* how often to try rd/wt before quitting */
#define NR_DEVICES        10	/* maximum number of drives */
#define MAX_WIN_RETRY  10000	/* max # times to try to output to WIN */
#define PART_TABLE     0x1C6	/* IBM partition table starts here in sect 0 */
#define DEV_PER_DRIVE      5	/* hd0 + hd1 + hd2 + hd3 + hd4 = 5 */

typedef struct disk_cmd {
	int ata;
	int sector;
	int offset;
	int bytes;
	char * buffer;
} disk_cmd;

void write(int ata, char * msg, int bytes, unsigned short sector, int offset);

void read(int ata, char * ans, int bytes, unsigned short sector, int offset);

unsigned short getStatusRegister(int ata);

void identifyDevice(int ata);

void check_drive(int ata);

unsigned short getErrorRegister(int ata);

#endif
