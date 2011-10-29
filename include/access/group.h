#ifndef GROUP_H
#define	GROUP_H

#include <defs.h>
#include <util/logger.h>

#define NO_GROUP -1


#define E_GROUP_INVALID_GROUPNAME -1;
#define E_GROUP_INVALID_PASSWORD -2;

#define GROUP_MAX 20
typedef struct {
    int uid;
    int gid;
    char groupName[32];
    char password[16];
} group_t;

typedef struct {
    int uid;
    int gid;
    char groupName[32];
} callgroup_t;

PUBLIC void group_init();
PUBLIC group_t *group_get(int uid);
PUBLIC int group_find(char *groupName);
PUBLIC group_t *group_login(int uid, char* password);
PUBLIC boolean group_string(int uid, char *string);
PUBLIC boolean do_groupadd(char *groupName, char *password);
PUBLIC boolean do_groupdel(char *groupName);
PUBLIC boolean do_groupsetgid(char *groupName, int gid);
PUBLIC boolean do_grouplist(callgroup_t *callgroups);
PUBLIC boolean group_isSet(int uid);
#define group_isValidUid(...) _isValidUid(__FILE__, __LINE__, __VA_ARGS__);

#endif	/* GROUP_H */
