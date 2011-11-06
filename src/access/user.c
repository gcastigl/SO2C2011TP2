#include <access/user.h>
#include <access/permission.h>

PRIVATE user_t users[USER_MAX];
PRIVATE fs_node_t *homeInode = NULL;
PRIVATE fs_node_t *passwd = NULL;

PRIVATE boolean user_parse(char *line);
PRIVATE void user_reset();
PRIVATE int user_findOpenUid();
PRIVATE boolean user_add(int uid);
PRIVATE boolean user_del(int uid);
PRIVATE boolean user_isValidFormat(char *token);
PRIVATE boolean createUserDir(int uid);
PRIVATE boolean deleteUserDir(int uid);
PRIVATE void flushList();
PRIVATE void loadList();

PRIVATE boolean user_isSet(int uid) {
    if (0 <= uid && uid < USER_MAX) {
        return users[uid].uid != NO_USER;
    }
    log(L_ERROR, "INVALID UID %d", uid);
    return false;
}

PRIVATE int user_findOpenUid() {
    for (int i = 0; i < USER_MAX; ++i) {
        if (!user_isSet(i)) {
            return i;
        }
    }
    return NO_USER;
}

PRIVATE boolean user_add(int uid) {
    if (!user_isSet(uid)) {
        users[uid].uid = uid;
        users[uid].gid = uid;
        users[uid].userName[0] = '\0';
        users[uid].password[0] = '\0';
        return true;
    } else {
        return false;
    }
}

PRIVATE boolean user_del(int uid) {
    user_t *user = user_get(uid);
    if (user != NULL) {
        user->uid = NO_USER;
        return true;
    } else {
        return false;
    }
}

PUBLIC user_t *user_get(int uid) {
    if (user_isSet(uid)) {
        return &users[uid];
    } else {
        return NULL;
    }
}

PUBLIC boolean do_userlist(calluser_t *callusers) {
    char userstring[128];
    for (int i = 0; i < USER_MAX; ++i) {
        user_string(i, userstring);
        log(L_DEBUG, "user %d: %s", i, userstring);
    }

    user_t *user = NULL;
    log(L_DEBUG, "douserlist");
    for (int i = 0; i < USER_MAX; ++i) {
        user = user_get(i);
        if (user == NULL) {
            callusers[i].uid = NO_USER;
            callusers[i].gid = NO_USER;
            strcpy(callusers[i].userName, "no-user");
        } else {
            callusers[i].uid = user->uid;
            callusers[i].gid = user->gid;
            strcpy(callusers[i].userName, user->userName);
        }
        log(L_DEBUG, "callu: %d:%d, %d:%d, %s:%s",
            user->uid, callusers[i].uid,
            user->gid, callusers[i].gid,
            user->userName, callusers[i].userName
        );
    }
    return true;
}

PUBLIC boolean do_usersetgid(char *userName, int gid) {
    int uid = user_find(userName);
    user_t *user = user_get(uid);
    if (user == NULL) {
        log(L_ERROR, "invalid user uid, %d: %s", uid, userName);
        return false;
    } else {
        log(L_INFO, "old: %d. new: %d", user->gid, gid);
        return user_setGid(uid, gid);
    }
}

PUBLIC boolean user_setUsername(int uid, char *username) {
    if (!user_isSet(uid)) {
        log(L_ERROR, "invalid uid %d", uid);
        return false;
    }
    if (username == NULL || !strlen(username)) {
        log(L_ERROR, "username cannot be empty");
        return false;
    }
    if (!user_isValidFormat(username)) {
        log(L_ERROR, "username cannot contain the ':' character");
        return false;
    }
    if (user_find(username) != NO_USER ) {
        log(L_ERROR, "username %s already exists", username);
        return false;
    }
    strcpy(user_get(uid)->userName, username);
    return true;
}

PUBLIC boolean user_setPassword(int uid, char *password) {
    if (!user_isSet(uid)) {
        log(L_ERROR, "invalid uid %d", uid);
        return false;
    }
    if (!user_isValidFormat(password)) {
        log(L_ERROR, "password cannot contain the ':' character");
        return false;
    }
    strcpy(user_get(uid)->password, password);
    return true;
}

PUBLIC boolean user_setGid(int uid, int gid) {
    if (!user_isSet(uid)) {
        log(L_ERROR, "invalid uid %d", uid);
        return false;
    }
    if (!group_isSet(gid)) {
        log(L_ERROR, "invalid gid %d", gid);
        return false;
    }
    user_get(uid)->gid = gid;
    return true;
}

PRIVATE void user_reset() {
    for (int i = 0; i < USER_MAX; ++i) {
        users[i].uid = NO_USER;
    }
}

PUBLIC boolean user_string(int uid, char *string) {
    user_t *user = user_get(uid);
    if (user) {
        sprintf(string, "%d:%d:%s:%s",
            user->uid,
            user->gid,
            user->userName,
            user->password
        );
        return true;
    } else {
        string[0] = '\0';
        return false;
    }
}

PUBLIC void user_init() {
    user_reset();
    fs_node_t root;
    fs_getRoot(&root);
    homeInode = finddir_fs(&root, "home");
    fs_node_t *etcnode = finddir_fs(&root, "etc");
    passwd = finddir_fs(etcnode, "passwd");
    user_parse("0:0:root:pass\n");
    user_parse("10:0:qcho:x\n");
    user_parse("11:11:qcho1:pass1\n");
    user_parse("12:12:qcho2:pass2");
    do_useradd("a", "a");
    do_useradd("guest", "");
    if (passwd == NULL) {
        fs_getFsNode(passwd, createdir_fs(etcnode, "passwd", FS_FILE));
        flushList();
    } else {
        loadList();
    }
    char userstring[128];
    for (int i = 0; i < USER_MAX; ++i) {
        user_string(i, userstring);
        log(L_DEBUG, "user %d: %s", i, userstring);
    }

    kfree(etcnode);
}

PRIVATE void loadList() {
    u8int buff[256];
    int offset = 0;
    int read;
    while((read = read_fs(passwd, offset, 256, buff)) != 0) {
        log(L_DEBUG, "passwd read: %s\n", buff);
        offset += read;
    }
}

PRIVATE void flushList() {
    // FIXME: delete all.
    int offset = 0;
    char userstring[128];
    for (int i = 0; i < USER_MAX; ++i) {
        user_string(i, userstring);
        write_fs(passwd, offset, strlen(userstring) + 1, (u8int*) userstring);
        offset += strlen(userstring) + 1;
        write_fs(passwd, offset++, 1, (u8int*)"\n");
    }
}

PRIVATE boolean user_parse(char *line) {
    enum { UID = 0, GID, USERNAME, PASSWORD} field = UID;
    boolean parsing_ok = true;
    char *p = strtok(line, ":");
    int uid = atoi(p);
    parsing_ok = user_add(atoi(p));
    while (p != NULL && parsing_ok) {
        p = strtok(NULL, ":\n");
        field++;
        switch (field) {
            case UID:
                // DONE.
                break;
            case GID:
                parsing_ok = user_setGid(uid, atoi(p));
                break;
            case USERNAME:
                parsing_ok = user_setUsername(uid, p);
                break;
            case PASSWORD:
                parsing_ok = user_setPassword(uid, p);
                break;
        }
    }
    if (parsing_ok) {
        parsing_ok = createUserDir(uid);
    }
    if (!parsing_ok && field != UID) {
        // delete invalid user.
        user_del(uid);
    }
    return parsing_ok;
}

PUBLIC int user_find(char *userName) {
    for (int i = 0; i < USER_MAX; ++i) {
        if (!strcmp(user_get(i)->userName, userName)) {
            return i;
        }
    }
    return NO_USER;
}

PUBLIC user_t *user_login(int uid, char* password) {
    user_t *user = user_get(uid);
    if (user != NULL) {
        if (!strcmp(user->password, password)) {
            return user;
        } else {
            errno = E_USER_INVALID_PASSWORD;
            return NULL;
        }
    } else {
        errno = E_USER_INVALID_USERNAME;
        return NULL;
    }
}

PRIVATE boolean user_isValidFormat(char *token) {
    for (int i = 0; i < strlen(token); ++i) {
        if (token[i] == ':') {
            return false;
        }
    }
    return true;
}

PUBLIC fs_node_t *user_getUserHomeInode(int uid) {
    return finddir_fs(homeInode, user_getName(uid));
}

PRIVATE boolean createUserDir(int uid) {
    user_t *user = user_get(uid);
    if (user == NULL) {
        return false;
    }
    errno = OK;
    int inode = createdir_fs(homeInode, user->userName, FS_DIRECTORY);
    if (inode == -1 && errno == E_FILE_EXISTS) {
        return true;
    } else if (errno == OK) {
        fs_setFileUid(inode, user->uid);
        fs_setFileGid(inode, user->gid);
        fs_setFileMode(inode, 0x600);
        return true;
    } else {
        return false;
    }
}

PRIVATE boolean deleteUserDir(int uid) {
    return false;
}

PUBLIC boolean do_useradd(char *userName, char *password) {
    boolean addOk = true;
    int uid = user_findOpenUid();
    user_add(uid);
    addOk = user_setUsername(uid, userName);
    addOk = user_setPassword(uid, password);
    addOk = user_setGid(uid, session_getEgid());
    if (addOk) {
        addOk = createUserDir(uid);
    }
    if (!addOk) {
        user_del(uid);
    }
    return addOk;
}

PUBLIC boolean do_userdel(char *userName) {
    int uid = user_find(userName);
    if (uid == NO_USER) {
        printf("No user exists with %s username.\n", userName);
        errno = INVALID_INPUT;
        return false;
    }
    user_t *user = user_get(uid);
    if (session_getEuid() == uid) {
        printf("You can't delete your own user.\n");
        errno = INVALID_INPUT;
        return false;
    }
    if (permission_user_isOwner(uid) || permission_group_isOwner(user->gid)) {
        return user_del(uid);
    } else {
        printf("Access denied.\n");
        errno = EACCES;
        return false;
    }
}

PUBLIC char *user_getName(int uid) {
    user_t *user = user_get(uid);
    if (user != NULL) {
        return user->userName;
    } else {
        return "unknown";
    }
}
