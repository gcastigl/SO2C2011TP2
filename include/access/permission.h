#ifndef PERMISSION_H
#define	PERMISSION_H

#include <defs.h>
#include <access/user.h>
#include <util/logger.h>
#include <fs/diskManager.h>
#include <fs/fs_API.h>

#define MASK_STRING_LEN 10+1

PUBLIC void mask_string(int perm, char *string);
PUBLIC boolean permission_user_hasAccess(int userId, int uid);
PUBLIC boolean permission_group_hasAccess(int gid, int uid);

#endif	/* PERMISSION_H */

