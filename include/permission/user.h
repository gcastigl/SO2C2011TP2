#ifndef USER_H
#define	USER_H

#include <defs.h>
#include <util/logger.h>

#define NO_USER -1

#define USER_MAX 20
#define E_USER_INVALID_USERNAME -1;
#define E_USER_INVALID_PASSWORD -2;

typedef struct {
    char userName[64];
    char password[32];
    int uid;
    int gid;
    char userInfo[64];
    char homePath[64];
} user_t;

typedef struct {
    int uid;
    int gid;
    char userName[64];
} calluser_t;

PUBLIC void user_init();
PUBLIC user_t *user_get(int uid);
PUBLIC int user_find(char *userName);
PUBLIC user_t *user_login(int uid, char* password);
PUBLIC boolean user_string(int uid, char *string);
PUBLIC boolean do_useradd(char *userName, char *password);
PUBLIC boolean do_userdel(char *userName);
PUBLIC boolean do_usersetgid(char *userName, int gid);
PUBLIC boolean do_userlist(calluser_t *callusers);
#define user_isValidUid(...) _isValidUid(__FILE__, __LINE__, __VA_ARGS__);

#endif	/* USER_H */

