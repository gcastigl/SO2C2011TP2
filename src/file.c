#include "../include/file.h"

void cd(const* path) {

}

int ls(char * arg) {

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
