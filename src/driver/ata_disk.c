#include <driver/ata_disk.h>
#include <asm/libasm.h>
#include <lib/stdlib.h>
#include <lib/stdio.h>
#include <util/logger.h>

#define BIT(i)	(1 << (i))

#define IS_REMOVABLE(D) 	printf("Is %sremovable\n", (D & BIT(7)) ? "" : "not ")
#define IS_ATA_DRIVE(D) 	printf("Is %sATA\n", (D & BIT(15)) ? "not " : "")
#define DMA_SUP(D) 			printf("DMA is %ssupported\n", (D & BIT(8)) ? "" : "not ")
#define LBA_SUP(D) 			printf("LBA is %ssupported\n", (D & BIT(9)) ? "" : "not")
#define DMA_QUEUED_SUP(D) 	printf("DMA QUEUED supported\n", (D & BIT(1)) ? "" : "not ")

void sendComm(int ata, int rdwr, unsigned short sector);
void _read(int ata, char * ans, unsigned short sector, int offset, int count);
void _write(int ata, char * msg, int bytes, unsigned short sector, int offset);

unsigned short getDataRegister(int ata);
void writeDataToRegister(int ata, char upper, char lower);

void translateBytes(char ans[], unsigned short sector);

void ata_normalize(unsigned short* sector, int* offset) {
	if (*offset >= SECTOR_SIZE) {
		*sector += (*offset / SECTOR_SIZE);
		*offset %= SECTOR_SIZE;
	}
}

void ata_read(int ata, void* msg, int bytes, unsigned short sector, int offset) {
	// log(L_DEBUG, "reading from ATA disk [%d, %d]", sector, offset);
	if (ata != ATA0 && ata != ATA1) {
		log(L_ERROR, "Trying to read from an inexistent disk!! %d - [%d, %d]", ata, sector, offset);
		errno = E_INVALID_ARG;
		return;
	}
	char* ans = (char*) msg;
	while (bytes != 0) {
		if (offset >= SECTOR_SIZE) {
			sector += (offset / SECTOR_SIZE);
			offset %= SECTOR_SIZE;
		}
		int size;
		if (offset + bytes > SECTOR_SIZE) {	// read remaming part from the sector
			size = SECTOR_SIZE - offset;
			_read(ata, ans, sector, offset, size);
			sector++;
			offset = 0;
			bytes -= size;
			ans += size;
		} else {					// The remaning msg fits in the actual sector
			size = bytes;
			_read(ata, ans, sector, offset, size);
			offset += size;
			bytes = 0;
			ans += size;
		}
	}
}

void _read(int ata, char * ans, unsigned short sector, int offset, int count) {
	char tmp[SECTOR_SIZE];
	sendComm(ata, LBA_READ, sector);
	// Now read sector
	int b;
	unsigned short data;
	for (b = 0;b < SECTOR_SIZE; b+=2) {
		data = getDataRegister(ata);
		translateBytes(tmp + b, data);
	}
	int i;
	for (i = 0;i < count; i++) {
		ans[i] = tmp[offset + i];
	}
}

void translateBytes(char * ans, unsigned short databyte) {
	ans[0] = databyte & 0xff;
	ans[1] = (databyte >> 8) & 0xFF;
}

void ata_write(int ata, void * msg, int bytes, unsigned short sector, int offset) {
	//	log(L_DEBUG, "writing to ATA disk [%d, %d]", sector, offset);
	if (ata != ATA0 && ata != ATA1) {
		log(L_ERROR, "Trying to write to an inexistent disk!! %d - [%d, %d]", ata, sector, offset);
		errno = E_INVALID_ARG;
		return;
	}
	char* ans = (char*) msg;
	while (bytes != 0) {
		if (offset >= SECTOR_SIZE) {
			sector += (offset / SECTOR_SIZE);
			offset %= SECTOR_SIZE;
		}
		int size;
		if (offset + bytes > SECTOR_SIZE) {	// Fill sector
			size = SECTOR_SIZE - offset;
			_write(ata, ans, size, sector, offset);
			sector++;
			offset = 0;
			bytes -= size;
			ans += size;
		} else {					// The remaning msg fits in the actual sector
			size = bytes;
			_write(ata, ans, size, sector, offset);
			offset += size;
			bytes = 0;
			ans += size;
		}
	}
}

void _write(int ata, char * msg, int bytes, unsigned short sector, int offset) {
	char tmp[SECTOR_SIZE];
	// Read actual sector because ATA always writes a complete sector!
	// Don't step previour values!
	_read(ata, tmp, sector, 0, SECTOR_SIZE);
	for (int i = 0; i < bytes; i++) {
		tmp[offset + i] = msg[i];
	}
	sendComm(ata, LBA_WRITE, sector);
	// Write updated sector
	for(int i = 0; i < SECTOR_SIZE; i += 2) {
		writeDataToRegister(ata, tmp[i + 1], tmp[i]);
	}
}

void writeDataToRegister(int ata, char upper, char lower) {
	unsigned short out;
	_cli();
	// Wait for driver's ready signal.
	while (!(_portw_in(ata + WIN_REG7) & BIT(3)))
		;
	out = upper << 8 | (lower & 0xff);
	_portw_out(ata + WIN_REG0, out);
	_sti();
}

unsigned short getDataRegister(int ata) {
	unsigned short ans;
	_cli();
	// Wait for driver's ready signal.
	while (!(_portw_in(ata + WIN_REG7) & BIT(3)))
	    ;
	ans = _portw_in(ata + WIN_REG0);
	_sti();
	return ans;
}

unsigned short getErrorRegister(int ata) {
	_cli();
	unsigned short rta = _port_in(ata + WIN_REG1) & 0x00000FFFF;
	_sti();
	return rta;
}

void sendComm(int ata, int rdwr, unsigned short sector) {
	_cli();
	_port_out(ata + WIN_REG1, 0);
	_port_out(ata + WIN_REG2, 0);	// Set count register sector in 1

	_port_out(ata + WIN_REG3, (unsigned char)sector);			// LBA low
	_port_out(ata + WIN_REG4, (unsigned char)(sector >> 8));    // LBA mid
	_port_out(ata + WIN_REG5, (unsigned char)(sector >> 16));	// LBA high
	_port_out(ata + WIN_REG6, 0xE0 | (ata << 4) | ((sector >> 24) & 0x0F));	// Set LBA bit in 1 and the rest in 0
	// Set command
	_port_out(ata + WIN_REG7, rdwr);
	_sti();
}

unsigned short ata_getStatusRegister(int ata) {
	unsigned short rta;
	_cli();
	rta = _port_in(ata + WIN_REG7) & 0x00000FFFF;
	_sti();
	return rta;
}

void identifyDevice(int ata) {
	_cli();
	_port_out(ata + WIN_REG6, 0);
	_port_out(ata + WIN_REG7, WIN_IDENTIFY);
	_sti();
}

// Check disk features
void ata_checkDrive(int ata) {
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

