#include <permission/user.h>

PRIVATE user_t *users[USER_MAX];

PRIVATE user_t *_parseUser(char* line);
PRIVATE void _resetUsers();
PRIVATE void _logUser(int uid);

PUBLIC boolean user_add(user_t *user) {
	if (user->uid >= 0 && user->uid >= USER_MAX) {
		log(L_ERROR, "{user_add} invalid uid: %d (%s)", user->uid, user->userName);
		return false;
	}
	if (users[user->uid] != NULL) {
		log(L_ERROR, "{user_add} uid %d already set: %s", user->uid, users[user->uid]->userName);
	}
	users[user->uid] = user;
	return true;
}

PRIVATE void _resetUsers() {
	for (int i = 0; i < USER_MAX; ++i) {
		users[i] = NULL;
	}
}

PRIVATE void _logUser(int uid) {
	user_t *user = user_find(uid);
	if (user == NULL) {
		log(L_DEBUG, "{_logUser} uid %s not set", uid);
	} else {
		log(L_DEBUG, "{_logUser} %s:%s:%d:%d:%s:s:s",
			user->userName,
			user->password,
			user->uid,
			user->gid,
			user->homePath,
			user->shellPath
		);
	}
}

PUBLIC void user_init() {
	log(L_INFO, "user init");
	_resetUsers();
	user_add(_parseUser("root:x:0:0:root:/root:/bin/bash"));
	user_add(_parseUser("qcho:x:10:10:Qcho:/home/qcho:/bin/bash"));

	for (int i = 0; i < USER_MAX; ++i) {
		_logUser(i);
	}
}

PUBLIC char *user_getName(int uid) {
	user_t *user = user_find(uid);
	if (user != NULL) {
		return user->userName;
	}
	return "";
}

PRIVATE user_t *_parseUser(char* line) {
    user_t *user;
    user = kmalloc(sizeof(user_t));
    sscanf(line, "%s:%s:%d:%d:%s:s:s",
        user->userName,
        user->password,
        user->uid,
        user->gid,
        user->homePath,
        user->shellPath
    );
    return user;
}

/**
 * returns the user or NULL if not exist.
 */
PUBLIC user_t *user_find(int uid) {
	if (users[uid] == NULL) {
		log(L_INFO, "tryed to access a non existant user, uid: %d", uid);
	}
	return users[uid];
}
