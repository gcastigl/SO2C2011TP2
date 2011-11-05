#ifndef PERMISSION_H
#define	PERMISSION_H

#include <defs.h>
#include <access/user.h>
#include <util/logger.h>
#include <fs/diskManager.h>
#include <fs/fs_API.h>
#include <session.h>

#define MASK_STRING_LEN 10+1

/**
 * Writes in string the file mask. Must have MASK_STRING_LEN allocated.
 */
PUBLIC void mask_string(int mask, char *string);

/**
 * Returns true if the currentUser is the owner of the user or root.
 */
PUBLIC boolean permission_user_isOwner(int uid);

/**
 * Returns true if the currentUser is the owner of the group or root.
 */
PUBLIC boolean permission_group_isOwner(int gid);

/**
 * Returns true if the currentUser is the owner of the file or root.
 */
PUBLIC boolean permission_file_isOwner(fs_node_t *node);

/**
 * Returns true if the currentUser has the desired access or is root.
 */
PUBLIC boolean permission_file_hasAccess(fs_node_t *node, int access_desired);

#endif	/* PERMISSION_H */

