#include <fs/file.h>

//BIG FIXME: this cmds should't be here....


// FIXME: idk how to keep track of the current path on the tty
int cd(int argc, char **argv) {
	if (argc == 1) {
		TTY* tty = tty_getCurrentTTY();
		u32int currentiNode = tty->currDirectory;
		fs_node_t current;
		fs_getFsNode(&current, currentiNode);
		fs_node_t *node = finddir_fs(&current, argv[0]);
		if (node != NULL) {
			if (node->flags == FS_DIRECTORY) {
				tty->currDirectory = node->inode;
			} else {
				printf("cd: %s is not a directory\n", argv[0]);
			}
		} else {
			printf("cd: The directory \"%s\" does not exist", argv[0]);
		}
	}
	return 0;
}

int ls(int argc, char **argv) {
	u32int currentiNode = tty_getCurrentTTY()->currDirectory;
	fs_node_t current;
	fs_getFsNode(&current, currentiNode);
	int i = 0;
	fs_node_t *node = 0;
	while ((node = readdir_fs(&current, i)) != 0) {					// get directory i
		printf("%d - %s", node->inode, node->name);
		if ((node->flags&0x7) == FS_DIRECTORY) {
			printf("\t(directory)\n");
		} else {
			printf("\t(file)\n");
		}
		i++;
	}
	return 0;
}

int mkdir(int argc, char **argv) {
	if(argc == 0 ) {
		printf("mkdir: missing operand\n");
	} else {
		int created = fs_createDirectory(tty_getCurrentTTY()->currDirectory, argv[0]);
		char* err = NULL;
		switch(created) {
			case 0:		// Directory was created OK
				break;
			case E_FILE_EXISTS:
				err = "Directory exists";
				break;
			default:
				err = "Unknown error";
		}
		if (err != NULL) {
			printf("mkdir: cannot create file %s: %s\n", argv[0], err);
		}
	}
	return 0;
}

int pwd(int argc, char **argv) {
	printf("%s\n", tty_getCurrentTTY()->currPath);
	return 0;
}

int touch(int argc, char **argv) {
	if(argc == 0 ) {
		printf("touch: missing operand\n");
	} else {
		int created = fs_createFile(tty_getCurrentTTY()->currDirectory, argv[0]);
		char* err = NULL;
		switch(created) {
			case 0:		// File was created OK
				break;
			case E_FILE_EXISTS:
				err = "File exists";
				break;
			default:
				err = "Unknown error";
		}
		if (err != NULL) {
			printf("touch: cannot create file %s: %s\n", argv[0], err);
		}
	}
	return 0;
}

int cat(int argc, char **argv) {
	TTY* tty = tty_getCurrentTTY();
	u32int currentiNode = tty->currDirectory;
	fs_node_t current;
	fs_getFsNode(&current, currentiNode);

	return 0;
}

int sysOpen(char* fileName, int oflags, int cflags) {
    int inode;
    if (oflags & O_CREAT) {
        // FIXME: SHOULD CONSIDER CFLAGS
        inode = fs_createFile(tty_getCurrentTTY()->currDirectory, fileName);
        if ((oflags & O_EXCL) && (inode == E_FILE_EXISTS)) {
            return ERROR;
        }
    }
    TTY* tty = tty_getCurrentTTY();
	u32int currentiNode = tty->currDirectory;
	fs_node_t current;
	fs_getFsNode(&current, currentiNode);
    return 0;
}
