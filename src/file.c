#include "../include/file.h"

int cd(int argc, char *argv[]) {
	if (argc != 1) {
		printf("This cmd must be called with one argument!\n");
		return 1;
	}
	TTY* currTTy = tty_getCurrentTTY();
	if (strcmp("..", argv[0]) == 0) { 				// Go up one direcotry
		Directory_t* next = currTTy->currDirectory->parent;
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
		Directory_t* next = directory_get(tty_getCurrentTTY()->currDirectory, argv[0]);
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
		Directory_t* current = tty_getCurrentTTY()->currDirectory;
		int i;
		for (i = 0; i < current->subDirsCount; i++) {
			printf("\t%s\n", current->subDirs[i]->name);
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
		int created = directory_create(tty_getCurrentTTY()->currDirectory, argv[0]);
		char* err = NULL;
		switch(created) {
			case 0:		// Directory was created OK
				break;
			case DIR_EXISTS:
				err = "File exists";
				break;
			case DIR_FULL:
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

/*
int mkdir(char * arg){
	if(strlen(arg) == 0 )
		printf("mkdir: invalid argument\n");
	else {
		if(openFile(arg, 0) == -1)
			printf("mkdir: file already exists\n");
	}
	return 0;
}

int cat(char * arg){
	File * file = getFileByName(arg);

	if(file == NULL) return -1;

	char * tmp = (char*) malloc(file->size);

	readFile(file, tmp, file->size);

	printf("File %s:\n", file->name);

	printf("%s\nEOF\n", tmp);

	return 0;
}


int touch( char * arg ){
	openFile(arg, 0);

	return 0;
}

int tree(char *a){

	File * file;

	if( strlen(a) == 0)
		file = __getCurrentDir();
	else file = getFileByName(a);

	if(file != NULL)
		__printFileTree(file);
	return 0;
}


int pwd(char * arg){

	File * file = __getCurrentDir();

	if(!strcmp(arg,"-a"))
		__printFile(file);
	else printFilePosition(file);

	printf("\n");
}

int rm(char * arg){

	File * curr = __getCurrentDir();
	File * file = __getChildByFileName(arg,curr);

	if(file!=NULL){
		__deleteFile(file);
		printf("File deleted\n");
	} else printf("rm: File not found\n");

}

int cd(char * arg){
	File * curr = __getCurrentDir();
	File * child = __getChildByFileName(arg,curr);

	if(strlen(arg) == 0)
		child = fileTable;

	if(child != NULL ){
		__setCurrentDir(child);
		return 0;
	} else if( !strcmp(arg,".")){
		// Do nothing
		return 0;
	} else if( !strcmp(arg,"..")){
		if(curr->parent!=NULL)
			__setCurrentDir(curr->parent);
		else printf("cd: %s: Root directory\n",curr->name);
		return 0;
	}

	printf("cd: %s: No such file or directory.\n", arg);
	return -1;
}

int ls(char * arg){

	File * file;
	if(strlen(arg) == 0)
		file = __getCurrentDir();
	else
		file = getFileByName(arg);

	if(file!=NULL){

		printf("ls: %s\n", file->name);

		printf(".\t..\t");

		int i;
		for(i=0;i<_FILE_CHILDREN;i++)
			if(file->children[i] != NULL){
				printf("%s\t", file->children[i]->name);
			}

		printf("\n");
		return 0;
	}

	printf("ls: not found\n");

	return -1;
}
*/
