#include <directory.h>

Directory_t root;

void initEmptyDirectory(Directory_t* dir, char* name);

void directory_initialize() {
	initEmptyDirectory(&root, "~");
}

int directory_create(Directory_t* dir, char* name) {
	if(directory_exists(dir, name)) {
		return DIR_EXISTS;
	} else if (dir->subDirsCount == MAX_FOLDERS_PER_FOLDER) {
		return DIR_FULL;
	}
	Directory_t* newFolder = (Directory_t*) kmalloc(sizeof(Directory_t));
	initEmptyDirectory(newFolder, name);
	newFolder->parent = dir;
	dir->subDirs[dir->subDirsCount++] = newFolder;
	return 0;
}

void initEmptyDirectory(Directory_t* dir, char* name) {
	int i;
	strcpy(dir->name, name);
	dir->filesCount = 0;
	for(i = 0; i < MAX_FILES_PER_FOLDER; i++) {
		dir->files[i] = NULL;
	}
	dir->subDirsCount = 0;
	for(i = 0; i < MAX_FOLDERS_PER_FOLDER; i++) {
		dir->subDirs[i] = NULL;
	}
}

Directory_t* directory_get(Directory_t* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return dir->subDirs[i];
		}
	}
	return NULL;
}

Directory_t* directory_getRoot() {
	return &root;
}

boolean directory_exists(Directory_t* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return true;
		}
	}
	return false;
}
