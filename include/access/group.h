#ifndef GROUP_H
#define	GROUP_H

#include <defs.h>
#include <util/logger.h>

#define NO_GROUP -1


#define E_GROUP_INVALID_GROUPNAME -1;
#define E_GROUP_INVALID_PASSWORD -2;

#define GROUP_MAX 15

typedef struct {
    int gid;
    char groupName[32];
    char password[16];
} group_t;

typedef struct {
    int gid;
    char groupName[32];
} callgroup_t;

PUBLIC void group_init();
PUBLIC group_t *group_get(int gid);
PUBLIC int group_find(char *groupName);
PUBLIC group_t *group_login(int gid, char* password);
PUBLIC boolean group_string(int gid, char *string);
PUBLIC boolean do_groupadd(char *groupName, char *password);
PUBLIC boolean do_groupdel(char *groupName);
PUBLIC boolean do_grouplist(callgroup_t *callgroups);
PUBLIC boolean group_isSet(int gid);
#define group_isValidGid(...) _isValidGid(__FILE__, __LINE__, __VA_ARGS__);
PUBLIC char *group_getName(int gid);

#endif	/* GROUP_H */
