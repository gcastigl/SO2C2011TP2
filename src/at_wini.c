#include <at_wini.h>

#define BIT(i)	(1 << (i))
/*
#define BIT0 1
#define BIT1 2
#define BIT2 4
#define BIT3 8
#define BIT4 16
#define BIT5 32
#define BIT6 64
#define BIT7 128
#define BIT8 256
#define BIT9 512
#define BIT10 1024
#define BIT11 2048
#define BIT12 4096
#define BIT13 8192
#define BIT14 16384
#define BIT15 32768
*/
#define IS_REMOVABLE(D) 	printf("Is %sremovable\n", (D & BIT(7)) ? "" : "not ")
#define IS_ATA_DRIVE(D) 	printf("Is %sATA\n", (D & BIT(15)) ? "not " : "")
#define DMA_SUP(D) 			printf("DMA is %ssupported\n", (D & BIT(8)) ? "" : "not ")
#define LBA_SUP(D) 			printf("LBA is %ssupported\n", (D & BIT(9)) ? "" : "not")
#define DMA_QUEUED_SUP(D) 	printf("DMA QUEUED supported\n", (D & BIT(1)) ? "" : "not ")

void sendComm(int ata, int rdwr, unsigned short sector);

void read(int ata, char * ans, unsigned short sector, int offset, int count);
void _read(int ata, char * ans, unsigned short sector, int offset, int count);

void write(int ata, char * msg, int bytes, unsigned short sector, int offset);
void _write(int ata, char * msg, int bytes, unsigned short sector, int offset);

unsigned short getDataRegister(int ata);
void writeDataToRegister(int ata, char upper, char lower);

void translateBytes(char ans[], unsigned short sector);

/* Namespace structure */
struct disk_t Disk = {
	read,
	write
};

void read(int ata, char * ans, unsigned short sector, int offset, int count) {
	int i;
	// Quantity of necessary sectors
	int sectors = ((count-1) / 512) + 1;
	for(i = 0;i < sectors; i++){
		int size =  (i == sectors - 1) ? count % 513 : 512;
		if(!i)
			_read(ata, ans, sector, offset, (offset + count > 512)? size - offset : size);
		else
			_read(ata, ans + (i * 512) - offset, sector + i, 0, size);
	}
}

// To read N bytes from hard disk, must alloc N+1 bytes for ans, as N+1 byte is used to null-character
void _read(int ata, char * ans, unsigned short sector, int offset, int count){

	// Just a sector...
	if(count > 512 - offset)
		return;

	char tmp[512];
	sendComm(ata, LBA_READ, sector);

	// Now read sector
	int b;
	unsigned short data;
	for (b = 0;b < 512; b+=2) {
		data = getDataRegister(ata);
		translateBytes(tmp + b, data);
	}
	int i;
	for (i = 0;i < count; i++)
		ans[i] = tmp[offset + i];
}

void translateBytes(char * ans, unsigned short databyte) {
	ans[0] = databyte & 0xFF;
	ans[1] = databyte >> 8;
}

// Wrapper para manejar muchos sectores
void write(int ata, char * msg, int bytes, unsigned short sector, int offset){
	int i;

	if(bytes == 0)
		return;

	// Quantity of necessary sectors
	int sectors = (bytes / 512) + 1;
	for(i=0;i<sectors;i++){
		int size =  (i == sectors - 1) ? bytes%513 : 512;
		// First sector, check offset
		if(!i)
			_write(ata, msg, (offset+bytes > 512)? size-offset :size, sector,offset);
		else
			_write(ata, msg + (i * 512) - offset, size, sector + i, 0);

	}
}

// Single sector write.
void _write(int ata, char * msg, int bytes, unsigned short sector, int offset){
	int i = 0;
	// Just a sector...
	if(bytes > 512 - offset)
		return;
	char tmp[512];
	// Read all sectors necessary
	_read(ata,tmp,sector,0,512);
	//DEBUG
/*	printf("Sector %d before:\n", sector);
	for(i=0;i<512;i++)
		putchar(tmp[i]);
	printf("\n");
	getchar();*/
	// Prepare sectors with new data
	for(i=0;i<bytes;i++)
		tmp[offset+i] = msg[i];
	//DEBUG
/*	printf("Sector %d after:\n", sector);
	printf("Offset %d\n", offset);
	for(i=0;i<512;i++)
		putchar(tmp[i]);
	printf("\n");
	getchar();*/
	// Send write command
	sendComm(ata, LBA_WRITE, sector);
	// Now write all sector
	int b;
	for(b = 0;b <= 512;b += 2)
		writeDataToRegister(ata, tmp[b+1], tmp[b]);
}

void writeDataToRegister(int ata, char upper, char lower){
	_Cli();
	unsigned short out;

	// Wait for driver's ready signal.
	while (!(_portw_in(ata + WIN_REG7) & BIT(3)))
		;

	out = (upper << 8) | lower;
	_portw_out(ata + WIN_REG0, out);

	_Sti();
}

unsigned short getDataRegister(int ata){
	_Cli();
	unsigned short ans;

	// Wait for driver's ready signal.
	while (!(_portw_in(ata + WIN_REG7) & BIT(3)));

	ans = _portw_in(ata + WIN_REG0);

	_Sti();
	return ans;
}

unsigned short getErrorRegister(int ata){
	_Cli();
	unsigned short rta = _port_in(ata + WIN_REG1) & 0x00000FFFF;
	_Sti();
	return rta;
}

void sendComm(int ata, int rdwr, unsigned short sector){
	_Cli();

	_port_out(ata + WIN_REG1, 0);
	_port_out(ata + WIN_REG2, 0);	// Set count register sector in 1

	_port_out(ata + WIN_REG3, (unsigned char)sector);			// LBA low
	_port_out(ata + WIN_REG4, (unsigned char)(sector >> 8));		// LBA mid
	_port_out(ata + WIN_REG5, (unsigned char)(sector >> 16));	// LBA high
	_port_out(ata + WIN_REG6, 0xE0 | (ata << 4) | ((sector >> 24) & 0x0F));	// Set LBA bit in 1 and the rest in 0

	// Set command
	_port_out(ata + WIN_REG7, rdwr);


	_Sti();
}

unsigned short getStatusRegister(int ata){
	unsigned short rta;
	_Cli();
	rta = _port_in(ata + WIN_REG7) & 0x00000FFFF;
	_Sti();
	return rta;
}

void identifyDevice(int ata){
	_Cli();
	_port_out(ata + WIN_REG6, 0);
	_port_out(ata + WIN_REG7, WIN_IDENTIFY);
	_Sti();
}

// Check disk features
void check_drive(int ata) {
	printf("-----------------------\n");
	printf("Identifying device ");
	switch(ata) {
		case ATA0:
			printf("ATA0...");
			break;
		case ATA1:
			printf("ATA1...");
			break;
	}
	printf("\n");
    identifyDevice(ata);
    unsigned short data = 0;
    int i;
    for (i = 0;i < 255; i++) {
        data = getDataRegister(ata);
		switch(i) {
			case 0:
				//printf("Data returned (%d): %d\n", i,data);
				IS_REMOVABLE(data);
				IS_ATA_DRIVE(data);
				break;
			case 49:
				DMA_SUP(data);
				LBA_SUP(data);
				break;
			case 83:
				DMA_QUEUED_SUP(data);
				break;
		}
    }
}
