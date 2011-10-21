#include <permission/permission.h>

void permission_set(iNode *inode, int mask) {
    inode->mask = mask;
}

boolean permission_hasAccess(iNode *inode, int uid) {
	if (uid == 0) {
		return true;
	}
    user_t *user = user_find(uid);
    int mask = inode->mask;
    return false;
}
