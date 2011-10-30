#include <access/permission.h>

PUBLIC void mask_string(int perm, char *string) {
	char type;

	if ((perm & FS_SOCKET) == FS_SOCKET) {
	    // Socket
	    type = 's';
	} else if ((perm & FS_SYMLINK) == FS_SYMLINK) {
	    // Enlace Simbólico
	    type = 'l';
	} else if ((perm & FS_FILE) == FS_FILE) {
	    // Regular
	    type = '-';
	} else if ((perm & FS_BLOCKDEVICE) == FS_BLOCKDEVICE) {
	    // Especial Bloque
	    type = 'b';
	} else if ((perm & FS_DIRECTORY) == FS_DIRECTORY) {
	    // Directorio
	    type = 'd';
	} else if ((perm & FS_CHARDEVICE) == FS_CHARDEVICE) {
	    // Especial Carácter
	    type = 'c';
	} else if ((perm & FS_PIPE) == FS_PIPE) {
	    // Tubería FIFO
	    type = 'p';
	} else {
	    // Desconocido
	    type = 'u';
	}

	string[0] = type;
	string[1] = ((perm & S_IRUSR) ? 'r' : '-');
	string[2] = ((perm & S_IWUSR) ? 'w' : '-');
	string[3] = ((perm & S_IXUSR) ? 'x' : '-');
	string[4] = ((perm & S_IRGRP) ? 'r' : '-');
	string[5] = ((perm & S_IWGRP) ? 'w' : '-');
	string[6] = ((perm & S_IXGRP) ? 'x' : '-');
	string[7] = ((perm & S_IROTH) ? 'r' : '-');
	string[8] = ((perm & S_IWOTH) ? 'w' : '-');
	string[9] = ((perm & S_IXOTH) ? 'x' : '-');
	string[10] = '\0';
}

PUBLIC boolean permission_user_hasAccess(int uid, int uidFrom, char *password) {
	if (uid == SUPER_USER || uid == uidFrom) {
		return true;
	}
	user_t *user = user_login(uid, password);
	return user != NULL;
}

PUBLIC boolean permission_group_hasAccess(int gid, int uid, char *password) {
	if (uid == SUPER_USER || uid == gid) {
		return true;
	}
	group_t *group = group_login(gid, password);
	return group != NULL;
}

void permission_file_set(iNode *inode, int mask) {
	inode->mask = mask;
}

boolean permission_file_hasAccess(iNode *inode, int uid, int access_desired) {
	int shift = 0, perm_bits = 0;
	if (uid == SUPER_USER) {
		return true;
	}
	user_t *user = user_get(uid);
	int bits = inode->mask;

	if (uid == inode->uid) {
		shift = 6; /* owner */
	} else if (user->gid == inode->gid) {
		shift = 3; /* group */
	} else {
		shift = 0; /* other */
	}
	perm_bits = (bits >> shift) & (R_BIT | W_BIT | X_BIT);

	/* If access desired is not a subset of what is allowed, it is refused. */
	int r = OK;
	if ((perm_bits | access_desired) != perm_bits)
		r = EACCES;

	/* Check to see if someone is trying to write on a file system that is
	 * mounted read-only.
	 */
//	if (r == OK) {
//	if (access_desired & W_BIT) {
//		r = read_only(rip);
//	}
//	}
//	if (rip != old_rip) put_inode(rip);
	return r;
}
