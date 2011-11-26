#include <session.h>

PRIVATE user_t *currentUser = NULL;

void prntWelcomeMsg();

void session_login() {
    TTY* tty = tty_getCurrentTTY();
    tty_clean(tty);
    prntWelcomeMsg();
    char password[PASS_MAX_LENGTH] = {"\0"};
    char user[NAME_MAX_LENGTH] = {"\0"};
    do {
        int uid = NO_USER;
        while (uid == NO_USER) {
            printf("\nUsername: ");
            gets(user);
            uid = user_find(user);
            if (uid == NO_USER) {
                printf("\tInvalid username!\n");
            }
        }
        int retry = 3;
        while (--retry && !session_isLoggedIn()) {
            printf("\nPassword: ");
            gets(password);
            currentUser = user_login(uid, password);
            if (currentUser == NULL) {
                printf("\tInvalid password!\n");
            } else {
                tty_setCurrentNode(*user_getUserHomeInode(uid));
            }
        }
        if (retry == 0) {
            printf("\tInvalid password! Please try again.\n");
        }
    } while(!session_isLoggedIn());

    printf("\nLogged in as: %s\n", session_getName());
}

int session_isLoggedIn() {
    return currentUser != NULL;
}

const char* session_getName() {
    if (session_isLoggedIn()) {
        return currentUser->userName;
    }
    return NULL;
}

void session_logout() {
    currentUser = NULL;
    fs_node_t root;
    fs_getRoot(&root);
    tty_setCurrentNode(root);
}

PUBLIC user_t *session_getCurrentUser() {
    return currentUser;
}

PUBLIC int session_getEuid() {
    if (currentUser != NULL) {
        return currentUser->uid;
    } else {
        return SUPER_USER;
    }
}

PUBLIC int session_getEgid() {
    if (currentUser != NULL) {
        return currentUser->gid;
    } else {
        return SUPER_USER;
    }
}

void prntWelcomeMsg() {
    TTY* currTty = tty_getCurrentTTY();
    char format = video_getFormattedColor(currTty->fgColor, currTty->bgColor); // current format backup
    currTty->bgColor = GREEN;
    currTty->fgColor = BLUE;
    printf(WELCOME_MSG);
    currTty->bgColor = video_getBGcolor(format);	// restore format
    currTty->fgColor = video_getFGcolor(format);
    printf("\n\n");
}
