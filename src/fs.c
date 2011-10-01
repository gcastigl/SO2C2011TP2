#include <fs.h>

#define FS_HEADER	"GAT_OS_FS"
static iNode iNodes[MAX_INODES];

void fs_create();
void fs_load();
boolean validate_header();
void write_header();


void fs_init() {
	printf("Initializing the file system...\n");
	if (validate_header()) {
		//fs_load();
		fs_create();
	} else {
		fs_create();
	}
	printf("OK!");
}

boolean validate_header() {
	int len = strlen(FS_HEADER);
	char header[len];
	ata_read(ATA0, header, len, 0, 0);
	return strcmp(header, FS_HEADER) == 0;
}

void write_header() {
	ata_write(ATA0, FS_HEADER, strlen(FS_HEADER), 0, 0);
}

void fs_load() {
	printf("loading a file system!\n");
}

void fs_create() {
	printf("Creating a new file system!\n");

	write_header();				// Save header for the next time the system starts...
	int i;
	for(i = 0; i < MAX_INODES; i++) {
		iNodes[i].contents = NULL;
	}
	// create root
	printf("Creating root directory...\n");
	directory_initialize();
	// create /dev
	printf("Creating /dev directory...\n");
	directory_create(directory_getRoot(), "dev");
}

int fs_persist(Directory_t* dir, int sector, int offset) {

}

void write_directory(Directory_t* dir, int sector, int secotOffset) {
	int offset = 0;
}
