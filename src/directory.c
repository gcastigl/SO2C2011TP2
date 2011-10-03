#include <directory.h>

Directory_t root;

void initEmptyDirectory(Directory_t* dir, char* name);
Directory_t* find_r(Directory_t* curr, char* name);

void directory_initialize() {
	initEmptyDirectory(&root, "~");
}

int directory_createDir(Directory_t* dir, char* name) {
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

int directory_createFile(Directory_t* dir, char* name) {
	if(directory_exists(dir, name)) {
		return DIR_EXISTS;
	} else if (dir->subDirsCount == MAX_FOLDERS_PER_FOLDER) {
		return DIR_FULL;
	}

	return 0;
}

void initEmptyDirectory(Directory_t* dir, char* name) {
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






