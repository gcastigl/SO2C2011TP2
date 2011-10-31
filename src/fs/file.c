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
			if ((node->mask&FS_DIRECTORY) == FS_DIRECTORY) {
				tty->currDirectory = node->inode;
				memcpy(tty->currPath, node->name, strlen(node->name) + 1);
			} else {
				printf("cd: %s is not a directory\n", argv[0]);
			}
			// free(node);
		} else {
			printf("cd: The directory \"%s\" does not exist\n", argv[0]);
		}
	}
	return 0;
}

int ls(int argc, char **argv) {
	u32int currentiNode = tty_getCurrentTTY()->currDirectory;
	fs_node_t current;
	fs_getFsNode(&current, currentiNode);
	int i = 0;
	fs_node_t *node = NULL;
	char perm[MASK_STRING_LEN] = "";
	if (argc == 0) {
		while ((node = readdir_fs(&current, i)) != 0) {					// get directory i
			mask_string(node->mask, perm);
			printf("%s\t%s\t%s\t%s%s\n",
				perm,
				user_getName(node->uid),
				group_getName(node->gid),
				node->name,
				((node->mask&FS_DIRECTORY) == FS_DIRECTORY) ? "/": ""
			);
			i++;
		}
	}
	return 0;
}

int mkdir(int argc, char **argv) {
	if(argc == 0 ) {
		printf("mkdir: missing operand\n");
	} else {
		errno = 0;
		fs_createFile(tty_getCurrentTTY()->currDirectory, argv[0], FS_DIRECTORY);
		char* err = NULL;
		switch(errno) {
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
	int ret;
	if(argc == 0 ) {
		printf("touch: missing operand\n");
	} else {
		errno = 0;
		fs_createFile(tty_getCurrentTTY()->currDirectory, argv[0], FS_FILE);
		ret = errno;
		char* err = NULL;
		switch(errno) {
			case 0:		// File was created OK
				break;
			case E_FILE_EXISTS:
				err = "File exists";
				break;
			default:
				err = "Unknown error";
		}
		if (errno != 0) {
			printf("touch: cannot create file %s: %s\n", argv[0], err);
		}
		if (argc == 2) {
			TTY* tty = tty_getCurrentTTY();
			u32int currentiNode = tty->currDirectory;
			fs_node_t current;
			fs_getFsNode(&current, currentiNode);
			fs_node_t* file = finddir_fs(&current, argv[0]);
			write_fs(file, 0, strlen(argv[1]) + 1, (u8int*) argv[1]);
			kfree(file);
		}
	}
	return ret;
}

int cat(int argc, char **argv) {
	if (argc == 1) {
		TTY* tty = tty_getCurrentTTY();
		u32int currentiNode = tty->currDirectory;
		fs_node_t current;
		fs_getFsNode(&current, currentiNode);
		fs_node_t* file = finddir_fs(&current, argv[0]);
		char* err = NULL;
		if (file == NULL) {
			err = "No such file or directory";
		} else if ((file->mask&FS_DIRECTORY) == FS_DIRECTORY) {
			err = "Is a directory";
		}
		if (err != NULL) {
			printf("cat: %s: %s\n", argv[0], err);
			return 0;
		}
		if ((file->mask&FS_SYMLINK) == FS_SYMLINK) {
			u32int link;
			read_fs(file, 0, sizeof(u32int), (u8int*) &link);
			printf("getting fs nonde: %d", link);
			fs_getFsNode(file, link);
			file->name[10] = '\0';
			printf("recursive with filename: %s\n", file->name);
			cat(1, (char**) &file->name);
			return 0;
			//fs_getFsNode(file, link);
		}
		u8int buff[512];
		int offset = 0;
		int read;
		while((read = read_fs(file, offset, 512, buff)) != 0) {
			printf("%s\n", buff);
			offset += read;
		}
		kfree(file);
	}
	return 0;
}

int ln(int argc, char **argv) {
	if (argc != 2) {
		printf("ln: missing file operand\n");
		return 0;
	}
	u32int currentiNode = tty_getCurrentTTY()->currDirectory;
	fs_node_t currentFsNode;
	fs_getFsNode(&currentFsNode, currentiNode);
	fs_node_t *target = finddir_fs(&currentFsNode, argv[0]);
	if (target == NULL) {
		printf("ln: accessing \"%s\": No such file or directory\n", argv[0]);
		return -1;
	}
	// create sym link
	u32int symLinkiNode = fs_createFile(currentiNode, argv[1], FS_SYMLINK);
	if (symLinkiNode == E_FILE_EXISTS) {
		printf("ln: accessing \"%s\": File exists\n", argv[1]);
		return -1;
	}
	fs_node_t symLink;
	fs_getFsNode(&symLink, symLinkiNode);
	// add contents to sym link
	write_fs(&symLink, 0, sizeof(u32int), (u8int*) &target->inode);
	kfree(target);
	return 0;
}

