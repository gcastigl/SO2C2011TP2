#include <access/permission.h>

PUBLIC void mask_string(int perm, char *string) {
	char type;
	if ((perm & FS_SOCKET) == FS_SOCKET) {
	    type = 's';
	} else if ((perm & FS_SYMLINK) == FS_SYMLINK) {
	    type = 'l';
	} else if ((perm & FS_FILE) == FS_FILE) {
	    type = '-';
	} else if ((perm & FS_BLOCKDEVICE) == FS_BLOCKDEVICE) {
	    type = 'b';
	} else if ((perm & FS_DIRECTORY) == FS_DIRECTORY) {
	    type = 'd';
	} else if ((perm & FS_CHARDEVICE) == FS_CHARDEVICE) {
	    type = 'c';
	} else if ((perm & FS_PIPE) == FS_PIPE) {
	    type = 'p';
	} else {
	    type = 'u'; // Desconocido
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

PUBLIC boolean permission_user_isOwner(int uid) {
	return (session_getEuid() == SUPER_USER || session_getEuid() == uid);
}

PUBLIC boolean permission_group_isOwner(int gid) {
	return (session_getEuid() == SUPER_USER || session_getEuid() == gid);
}

PUBLIC boolean permission_file_isOwner(fs_node_t node) {
    return (session_getEuid() == SUPER_USER || session_getEuid() == node.uid);
}

PUBLIC boolean permission_file_hasAccess(fs_node_t node, int access_desired) {
	int shift = 0, perm_bits = 0;
	if (session_getEuid() == SUPER_USER) {
		return true;
	}
	user_t *user = user_get(session_getEuid());
	int bits = node.mask;

	if (session_getEuid() == node.uid) {
		shift = 6; /* owner */
	} else if (user->gid == node.gid) {
		shift = 3; /* group */
	} else {
		shift = 0; /* other */
	}
	perm_bits = (bits >> shift) & (R_BIT | W_BIT | X_BIT);
	return (perm_bits | access_desired) == perm_bits;
}
