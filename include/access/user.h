#ifndef USER_H
#define	USER_H

#include <defs.h>
#include <util/logger.h>
#include <access/group.h>
#include <fs/fs.h>


#define NO_USER -1
#define SUPER_USER 0

#define E_USER_INVALID_USERNAME -1;
#define E_USER_INVALID_PASSWORD -2;

#define USER_MAX 20
typedef struct {
    int uid;
    int gid;
    char userName[32];
    char password[16];
} user_t;

typedef struct {
    int uid;
    int gid;
    char userName[32];
} calluser_t;

/* user_init
*
* Inicializa los usuarios
**/
PUBLIC void user_init();

/* user_get
*
* Recibe como parametros:
* - User ID
*
* Devuelve el usuario con user id uid
**/

PUBLIC user_t *user_get(int uid);

/* user_find
*
* Recibe como parametros:
* - User name
*
* Devuelve el usuario con el nombre userName
**/
PUBLIC int user_find(char *userName);

/* user_setGid
*
* Recibe como parametros:
* - User ID
* - Group ID
*
* Setea el grupo del user uid
**/
PUBLIC boolean user_setGid(int uid, int gid);

/* user_login
*
* Recibe como parametros:
* - User Id
* - Password
*
* Loguea al usuario uid
**/
PUBLIC user_t *user_login(int uid, char* password);
PUBLIC boolean user_string(int uid, char *string);

/* do_useradd
*
* Recibe como parametros:
* - User Name
* - Password
*
* Agrega al usuario 'userName' con password 'password'
**/
PUBLIC boolean do_useradd(char *userName, char *password);

/* do_userdel
*
* Recibe como parametros:
* -User name
*
* Borra al usuario 'userName'
**/
PUBLIC boolean do_userdel(char *userName);

/* do_usersetgid
*
* Recibe como parametros:
* - User Name
* - Group Id
*
* Setea el grupo del usuario 'userName'
**/
PUBLIC boolean do_usersetgid(char *userName, int gid);

/* do_userlist
*
* Recibe como parametros:
* - Estructura que inicializar
*
* Inicializa callusers con los usuarios del sistema
**/
PUBLIC boolean do_userlist(calluser_t *callusers);
#define user_isValidUid(...) _isValidUid(__FILE__, __LINE__, __VA_ARGS__);

/* user_getName
*
* Recibe como parametros:
* - User ID
*
* Devuelve el nombre del user con id 'uid'
**/
PUBLIC char *user_getName(int uid);

/**
 * Devuelve el inodo de la home folder /home/username
 */
PUBLIC fs_node_t *user_getUserHomeInode(int uid);

#endif

