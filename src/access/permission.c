#include <access/permission.h>

void permission_set(iNode *inode, int mask) {
	inode->mask = mask;
}

boolean permission_hasAccess(iNode *inode, int uid, int access_desired) {
	int shift = 0, perm_bits = 0;
	if (uid == 0) {
		return true;
	}
	user_t *user = user_find(uid);
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
