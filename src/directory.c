#include <directory.h>

static Directory_t root;

static FileTable files[FILES_TABLES_SIZE];

static void initEmptyDirectory(Directory_t* dir, char* name);
static boolean fileExists(FileTable* fileTable, char* fileName);
Directory_t* find_r(Directory_t* curr, char* name);

void directory_initialize() {
	initEmptyDirectory(&root, "~");
	int i;
	for(i = 0; i < FILES_TABLES_SIZE; i++) {
		files[i].dir = NULL;
		files[i].filesCount = 0;
	}
}

int directory_createDir(Directory_t* dir, char* name) {
	if(directory_exists(dir, name)) {
		return E_DIR_EXISTS;
	} else if (dir->subDirsCount == MAX_FOLDERS_PER_FOLDER) {
		return E_DIR_FULL;
	}
	Directory_t* newFolder = (Directory_t*) kmalloc(sizeof(Directory_t));
	initEmptyDirectory(newFolder, name);
	newFolder->parent = dir;
	dir->subDirs[dir->subDirsCount++] = newFolder;
	return 0;
}

int directory_createFile(Directory_t* dir, char* fileName) {
	if (fileExists(dir->fileTable, fileName)) {
		return E_DIR_EXISTS;
	} else if (dir->fileTable->filesCount == MAX_FILES_PER_FOLDER - 1) {
		return E_DIR_FULL;
	}
	iNode* file = (iNode*) kmalloc(sizeof(iNode));
	int index = dir->fileTable->filesCount;
	strcpy(file->name, fileName);
	dir->fileTable->files[index] = file;
	file->contents = NULL;
	file->contentsLength = 0;
	file->used = 0;
	dir->fileTable->filesCount++;
	return 0;
}

static boolean fileExists(FileTable* fileTable, char* fileName) {
	int i;
	for (i = 0; i < fileTable->filesCount; i++) {
		if (strcmp(fileTable->files[i]->name, fileName) == 0) {
			return true;
		}
	}
	return false;
}

static void initEmptyDirectory(Directory_t* dir, char* name) {
	int i;
	strcpy(dir->name, name);
	dir->filesCount = 0;
	dir->subDirsCount = 0;
	for(i = 0; i < MAX_FOLDERS_PER_FOLDER; i++) {
		dir->subDirs[i] = NULL;
	}
}

// Devuelve (en el caso de existir) el subdirectorio con nombre "name". NULL en caso contrario.
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

void directory_setRoot(Directory_t* dir) {
	memcpy(&root, dir, sizeof(Directory_t));
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

// Busca a partir del directorio from el directorio con nombre "name" y lo devuelve.
// Si el pramatero en NULL, comienza la busqueda desde el directorio root.
// Retorna NULL en caso de no existir el directorio.
Directory_t* directory_find(Directory_t* from, char* name) {
	if (from == NULL) {
		from = &root;
	}
	return find_r(from, name);
}

Directory_t* find_r(Directory_t* curr, char* name) {
	if (strcmp(curr->name, name) == 0) {
		return curr;
	}
	int i;
	Directory_t* found;
	for (i = 0; i < curr->subDirsCount; i++) {
		found = find_r(curr->subDirs[i], name);
		if (found != NULL) {
			return found;
		}
	}
	return NULL;
}






