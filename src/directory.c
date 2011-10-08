#include <directory.h>

static Directory root;

static FileTableEntry fileTable[FILE_TABLE_SIZE];
static int filesCurrentIndex;

static boolean entryContainsFile(FileTableEntry* tableEntry, char* fileName);
Directory* find_r(Directory* curr, char* name);
static void createFileTableEntry(Directory* dir);
static void freeINodeResources(iNode** files, int length);

void directory_initialize() {
	filesCurrentIndex = 0;
}

int directory_createDir(Directory* dir, char* name) {
	if(directory_exists(dir, name)) {
		return E_DIR_EXISTS;
	} else if (dir->subDirsCount == MAX_FOLDERS_PER_FOLDER) {
		return E_DIR_FULL;
	}
	Directory* newDirectory = (Directory*) kmalloc(sizeof(Directory));
	initEmptyDirectory(newDirectory, name);
	newDirectory->parent = dir;
	dir->subDirs[dir->subDirsCount++] = newDirectory;
	return 0;
}

void initEmptyDirectory(Directory* dir, char* name) {
	strcpy(dir->name, name);
	dir->subDirsCount = 0;
	createFileTableEntry(dir);
}

int directory_createFile(Directory* dir, char* fileName) {
	if (entryContainsFile(dir->fileTableEntry, fileName)) {
		return E_DIR_EXISTS;
	} else if (dir->fileTableEntry->filesCount == MAX_FILES_PER_FOLDER - 1) {
		return E_DIR_FULL;
	}
	if (dir->fileTableEntry->files == NULL) {
		dir->fileTableEntry->files = (iNode**) kmalloc(sizeof(iNode*) * MAX_FILES_PER_FOLDER);
	}
	iNode* file = (iNode*) kmalloc(sizeof(iNode));
	strcpy(file->name, fileName);
	file->contents = NULL;
	file->contentsLength = 0;
	file->used = 0;
	dir->fileTableEntry->files[dir->fileTableEntry->filesCount] = file;
	dir->fileTableEntry->filesCount++;
	return 0;
}

static boolean entryContainsFile(FileTableEntry* tableEntry, char* fileName) {
	int i;
	for (i = 0; i < tableEntry->filesCount; i++) {
		if (strcmp(tableEntry->files[i]->name, fileName) == 0) {
			return true;
		}
	}
	return false;
}

// Devuelve (en el caso de existir) el subdirectorio con nombre "name". NULL en caso contrario.
Directory* directory_get(Directory* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return dir->subDirs[i];
		}
	}
	return NULL;
}

Directory* directory_getRoot() {
	return &root;
}

void directory_setRoot(Directory* dir) {
	memcpy(&root, dir, sizeof(Directory));
}

boolean directory_exists(Directory* dir, char* name) {
	int i;
	for (i = 0; i < dir->subDirsCount; i++) {
		if (strcmp(dir->subDirs[i]->name, name) == 0) {
			return true;
		}
	}
	return false;
}

// Busca a partir del directorio from el directorio con nombre "name" y lo devuelve.
// Si el pramatero en NULL, comienza la busqueda desde el directorio root.
// Retorna NULL en caso de no existir el directorio.
Directory* directory_find(Directory* from, char* name) {
	if (from == NULL) {
		from = &root;
	}
	return find_r(from, name);
}

Directory* find_r(Directory* curr, char* name) {
	if (strcmp(curr->name, name) == 0) {
		return curr;
	}
	int i;
	Directory* found;
	for (i = 0; i < curr->subDirsCount; i++) {
		found = find_r(curr->subDirs[i], name);
		if (found != NULL) {
			return found;
		}
	}
	return NULL;
}


static void createFileTableEntry(Directory* dir) {
	if (fileTable[filesCurrentIndex].dir == NULL) {
		fileTable[filesCurrentIndex].files = (iNode**) kmalloc(sizeof(iNode*) * MAX_FILES_PER_FOLDER);
	} else {
		// There is another directory using this entry, free entry resources
		freeINodeResources(fileTable[filesCurrentIndex].files, MAX_FILES_PER_FOLDER);
	}
	dir->fileTableEntry = fileTable + filesCurrentIndex;
	fileTable[filesCurrentIndex].dir = dir;
	fileTable[filesCurrentIndex].filesCount = 0;
	fileTable[filesCurrentIndex].files = NULL;
	filesCurrentIndex++;
	filesCurrentIndex %= FILE_TABLE_SIZE;
}

static void freeINodeResources(iNode** files, int length) {
	int i;
	for (i = 0; i < length; i++) {
		if (files[i]->contents != NULL) {
			kfree(files[i]->contents);
		}
	}
}


