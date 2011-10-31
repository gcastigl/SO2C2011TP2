#ifndef PERMISSION_H
#define	PERMISSION_H

#include <defs.h>
#include <access/user.h>
#include <util/logger.h>
#include <fs/diskManager.h>
#include <fs/fs_API.h>
#include <session.h>

#define MASK_STRING_LEN 10+1

PUBLIC void mask_string(int perm, char *string);
PUBLIC boolean permission_user_isOwner(int uid);
PUBLIC boolean permission_group_isOwner(int gid);
PUBLIC boolean permission_file_isOwner(fs_node_t node);
PUBLIC boolean permission_file_hasAccess(fs_node_t node, int access_desired);

#endif	/* PERMISSION_H */

