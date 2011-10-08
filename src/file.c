#include "../include/file.h"

//FIXME: this cmds should't be here....
int cd(int argc, char *argv[]) {
	if (argc != 1) {
		printf("This cmd must be called with one argument!\n");
		return 1;
	}
	TTY* currTTy = tty_getCurrentTTY();
	if (strcmp("..", argv[0]) == 0) { 				// Go up one direcotry
		Directory* next = currTTy->currDirectory->parent;
		if (next != NULL) {
			int folderNameLen = strlen(currTTy->currDirectory->name);
			currTTy->currPathOffset -= folderNameLen + 1;
			currTTy->currPath[currTTy->currPathOffset] = '\0';
			currTTy->currDirectory = next;
		}
	} else if (argv[0][0] == '/') {					// Absolute path directory
		//TODO:
		printf("Not implemented yet...\n");
	} else {										// Relative path
		Directory* next = directory_get(tty_getCurrentTTY()->currDirectory, argv[0]);
		if (next != NULL) { // Switch directory (advance one folder)
			currTTy->currDirectory = next;
			int offset = currTTy->currPathOffset;
			strcpy(currTTy->currPath + offset++, "/");
			strcpy(currTTy->currPath + offset, next->name);
			offset += strlen(next->name);
			currTTy->currPathOffset = offset;
		} else {
			printf("cd: The directory %s does not exist\n", argv[0]);
		}
	}
	return 0;
}

int ls(int argc, char *argv[]) {
	if (argc == 0) {
		Directory* current = tty_getCurrentTTY()->currDirectory;
		int i;
		tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));
		for (i = 0; i < current->subDirsCount; i++) {
			printf("\t%s\n", current->subDirs[i]->name);
			//printf("\t\tThis directory has %d files\n", current->subDirs[i]->fileTableEntry->filesCount);
		}
		tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_GREEN, BLACK));
		for (i = 0; i < current->fileTableEntry->filesCount; i++) {
			printf("\t%s\n", current->fileTableEntry->files[i]->name);
		}
		return 0;
	}
	if (strcmp(argv[0], "/") == 0) {				// Absolute path
		//TODO:
		printf("Not implemented yet...\n");
	} else {										// Relative path
		//TODO:
		printf("Not implemented yet...\n");
	}
	return 0;
}

int mkdir(int argc, char *argv[]) {
	if(argc == 0 ) {
		printf("mkdir: missing operand\n");
	} else {
		int created = fs_createDirectory(tty_getCurrentTTY()->currDirectory, argv[0]);
		char* err = NULL;
		switch(created) {
			case 0:		// Directory was created OK
				break;
			case E_DIR_EXISTS:
				err = "Directory exists";
				break;
			case E_DIR_FULL:
				err = "Directory is full";
				break;
			default:
				err = "Unknown error";
		}
		if (err != NULL) {
			printf("mkdir: cannot create directory %s: %s\n", argv[0], err);
		}
	}
	return 0;
}

int pwd(int argc, char *argv[]) {
	printf("%s\n", tty_getCurrentTTY()->currPath);
	return 0;
}

int touch(int argc, char *argv[]) {
	if(argc == 0 ) {
		printf("mkdir: missing operand\n");
	} else {
		int created = fs_createFile(tty_getCurrentTTY()->currDirectory, argv[0]);
		char* err = NULL;
		switch(created) {
			case 0:		// Directory was created OK
				break;
			case E_DIR_EXISTS:
				err = "File exists";
				break;
			case E_DIR_FULL:
				err = "File is full";
				break;
			case E_OUT_OF_MEMORY:
				err = "Out of memory";
				break;
			default:
				err = "Unknown error";
		}
		if (err != NULL) {
			printf("touch: cannot create File %s: %s\n", argv[0], err);
		}
	}
	return 0;
}

int cat(int argc, char *argv[]) {
	return 0;
}

