#include <access/permission.h>

PUBLIC void mask_string(int mask, char *string) {
    char type;
    if ((mask & FS_SOCKET) == FS_SOCKET) {
        type = 's';
    } else if ((mask & FS_SYMLINK) == FS_SYMLINK) {
        type = 'l';
    } else if ((mask & FS_FILE) == FS_FILE) {
        type = '-';
    } else if ((mask & FS_BLOCKDEVICE) == FS_BLOCKDEVICE) {
        type = 'b';
    } else if ((mask & FS_DIRECTORY) == FS_DIRECTORY) {
        type = 'd';
    } else if ((mask & FS_CHARDEVICE) == FS_CHARDEVICE) {
        type = 'c';
    } else if ((mask & FS_PIPE) == FS_PIPE) {
        type = 'p';
    } else {
        type = 'u'; // Desconocido
    }
    string[0] = type;
    string[1] = ((mask & S_IRUSR) ? 'r' : '-');
    string[2] = ((mask & S_IWUSR) ? 'w' : '-');
    string[3] = ((mask & S_IXUSR) ? 'x' : '-');
    string[4] = ((mask & S_IRGRP) ? 'r' : '-');
    string[5] = ((mask & S_IWGRP) ? 'w' : '-');
    string[6] = ((mask & S_IXGRP) ? 'x' : '-');
    string[7] = ((mask & S_IROTH) ? 'r' : '-');
    string[8] = ((mask & S_IWOTH) ? 'w' : '-');
    string[9] = ((mask & S_IXOTH) ? 'x' : '-');
    string[10] = '\0';
}

PUBLIC boolean permission_user_isOwner(int uid) {
    return (session_getEuid() == SUPER_USER || session_getEuid() == uid);
}

PUBLIC boolean permission_group_isOwner(int gid) {
    return (session_getEuid() == SUPER_USER || session_getEuid() == gid);
}

PUBLIC boolean permission_file_isOwner(fs_node_t* node) {
    return (session_getEuid() == SUPER_USER || session_getEuid() == node->uid);
}

PUBLIC boolean permission_file_hasAccess(fs_node_t node, int access_desired) {
    if (session_getEuid() == SUPER_USER) {
        return true;
    }
    user_t *user = user_get(session_getEuid());

    if (session_getEuid() == node.uid) {
        access_desired <<= 8; /* owner */
    } else if (user->gid == node.gid) {
        access_desired <<= 4; /* group */
    } else {
        access_desired <<= 0; /* other */
    }
    //	log(L_DEBUG, "%x and %x", node.mask, access_desired);
    return (node.mask & access_desired) == access_desired;
}
