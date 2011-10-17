#ifndef USER_H
#define	USER_H

#include <defs.h>
#include <util/logger.h>

#define USER_MAX 20

typedef struct {
    char *userName;
    char *password;
    int uid;
    int gid;
    char *userInfo;
    char *homePath;
    char *shellPath;
} user_t;

PUBLIC void user_init();
PUBLIC user_t *user_find(int uid);
PUBLIC char *user_getName(int uid);

#endif	/* USER_H */

