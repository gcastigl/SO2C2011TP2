#include <access/permission.h>

PUBLIC void mask_string(int perm, char *string) {
	char type;

	if ((perm & 0xC000) == 0xC000) {
	    // Socket
	    type = 's';
	} else if ((perm & 0xA000) == 0xA000) {
	    // Enlace Simbólico
	    type = 'l';
	} else if ((perm & 0x8000) == 0x8000) {
	    // Regular
	    type = '-';
	} else if ((perm & 0x6000) == 0x6000) {
	    // Especial Bloque
	    type = 'b';
	} else if ((perm & 0x4000) == 0x4000) {
	    // Directorio
	    type = 'd';
	} else if ((perm & 0x2000) == 0x2000) {
	    // Especial Carácter
	    type = 'c';
	} else if ((perm & 0x1000) == 0x1000) {
	    // Tubería FIFO
	    type = 'p';
	} else {
	    // Desconocido
	    type = 'u';
	}

	string[0] = type;
	// Propietario
	string[1] = ((perm & 0x0100) ? 'r' : '-');
	string[2] = ((perm & 0x0080) ? 'w' : '-');
	string[3] = ((perm & 0x0040) ?
			((perm & 0x0800) ? 's' : 'x' ) :
			((perm & 0x0800) ? 'S' : '-'));

	// Grupo
	string[4] = ((perm & 0x0020) ? 'r' : '-');
	string[5] = ((perm & 0x0010) ? 'w' : '-');
	string[6] = ((perm & 0x0008) ?
	            ((perm & 0x0400) ? 's' : 'x' ) :
	            ((perm & 0x0400) ? 'S' : '-'));

	// Mundo
	string[7] = ((perm & 0x0004) ? 'r' : '-');
	string[8] = ((perm & 0x0002) ? 'w' : '-');
	string[9] = ((perm & 0x0001) ?
	            ((perm & 0x0200) ? 't' : 'x' ) :
	            ((perm & 0x0200) ? 'T' : '-'));
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
