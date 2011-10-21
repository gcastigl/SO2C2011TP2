#ifndef USER_H
#define	USER_H

#include <defs.h>
#include <util/logger.h>

#define USER_MAX 20
#define E_USER_INVALID_USERNAME -1;
#define E_USER_INVALID_PASSWORD -2;

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
PUBLIC user_t *user_get(int uid);
PUBLIC boolean user_exists(char *userName);
PUBLIC user_t *user_login(char* userName, char* password);

#endif	/* USER_H */

