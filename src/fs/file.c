#include <fs/file.h>

//FIXME: this cmds should't be here....
int cd(int argc, char *argv[]) {
	if (argc == 1) {
		TTY* tty = tty_getCurrentTTY();
		u32int currentiNode = tty->currDirectory;
		fs_node_t current;
		fs_getFsNode(&current, currentiNode);
		fs_node_t *node = finddir_fs(&current, argv[0]);
		if (node != NULL) {
			tty->currDirectory = node->inode;
		}
	}
	return 0;
}

int ls(int argc, char *argv[]) {
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
			printf("\tcontents:\n");
			/*char buf[256];
			u32int sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++)
				monitor_put(buf[j]);*/
		}
		i++;
	}
	return 0;
}

int mkdir(int argc, char *argv[]) {
	/*if(argc == 0 ) {
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
	}*/
	return 0;
}

int pwd(int argc, char *argv[]) {
	printf("%s\n", tty_getCurrentTTY()->currPath);
	return 0;
}

int touch(int argc, char *argv[]) {
	/*if(argc == 0 ) {
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
	}*/
	return 0;
}

int cat(int argc, char *argv[]) {
	return 0;
}

