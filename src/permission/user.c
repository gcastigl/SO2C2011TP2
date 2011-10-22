#include <permission/user.h>

PRIVATE user_t *users[USER_MAX];

PRIVATE user_t *_parseUser(char* line);
PRIVATE void _resetUsers();
PRIVATE void _logUser(int uid, char* prefix);

PUBLIC boolean user_add(user_t *user) {
	if (user->uid >= 0 && user->uid >= USER_MAX) {
		log(L_ERROR, "{user_add} invalid uid: %d (%s)", user->uid, user->userName);
		return false;
	}
	if (user_get(user->uid) != NULL) {
		log(L_ERROR, "{user_add} uid %d already set: %s", user->uid, user_get(user->uid)->userName);
		return false;
	}
	users[user->uid] = user;
	return true;
}

PRIVATE void _resetUsers() {
	for (int i = 0; i < USER_MAX; ++i) {
		users[i] = NULL;
	}
}

PRIVATE void _logUser(int uid, char* prefix) {
	user_t *user = user_get(uid);
	if (user != NULL) {
		log(L_DEBUG, "{%s uid %d} %s_%s_%d_%d_%s_%s",
			prefix,
			uid,
			user->userName,
			user->password,
			user->uid,
			user->gid,
			user->userInfo,
			user->homePath,
			user->shellPath
		);
	}
}

PUBLIC void user_init() {
	//log(L_INFO, "user init");
	_resetUsers();
	user_add(_parseUser("qcho3:x:12:10:Qcho:/home/qcho:/bin/bash"));
	user_add(_parseUser("root:x:0:0:root:/root:/bin/bash"));
	user_add(_parseUser("qcho:x:10:10:Qcho:/home/qcho:/bin/bash"));
	user_add(_parseUser("qcho2:x:11:10:Qcho:/home/qcho:/bin/bash"));
//	for (int i = 0; i < USER_MAX; ++i) {
//		_logUser(i, "init");
//	}
}

PRIVATE user_t *_parseUser(char* line) {
	user_t *user = (user_t *)kmalloc(sizeof(user_t));
	enum { USERNAME = 0, PASSWORD, UID, GID, USERINFO, HOMEPATH, SHELLPATH} item;
	int i = 0;
	char *buff = kmalloc(64 * sizeof(char));
	int start = 0;
	int len;
	char c;
	item = USERNAME;
	while ((c = line[i++]) != '\0') {
		if (c == ':') {
			len = i-start-1;
			switch (item) {
				case USERNAME:
					user->userName = kmalloc(len * sizeof(char));
					strncpy(user->userName, line+start, len);
					log(L_TRACE, "userName: %s", user->userName);
					break;
				case PASSWORD:
					user->password = kmalloc(len * sizeof(char));
					strncpy(user->password, line+start, len);
					log(L_TRACE, "password: %s", user->password);
					break;
				case UID:
					strncpy(buff, line+start, len);
					user->uid = atoi(buff);
					log(L_TRACE, "uid: %d", user->uid);
					break;
				case GID:
					strncpy(buff, line+start, len);
					user->gid = atoi(buff);
					log(L_TRACE, "gid: %d", user->gid);
					break;
				case USERINFO:
					user->userInfo = kmalloc(len * sizeof(char));
					strncpy(user->userInfo, line+start, len);
					log(L_TRACE, "userInfo: %s", user->userInfo);
					break;
				case HOMEPATH:
					user->homePath = kmalloc(len * sizeof(char));
					strncpy(user->homePath, line+start, len);
					log(L_TRACE, "homePath: %s", user->homePath);
					break;
				case SHELLPATH:
					user->shellPath = kmalloc(len * sizeof(char));
					strncpy(user->shellPath, line+start, len);
					log(L_TRACE, "shellPath: %s", user->shellPath);
					break;
				default:
					return user;
			}
			start = i;
			item++;
		}
	}
    return user;
}

/**
 * returns the user or NULL if not exist.
 */
PUBLIC user_t *user_get(int uid) {
	return users[uid];
}

PUBLIC boolean user_exists(char *userName) {
	log(L_DEBUG, "{user_exists} test: %s", userName);
	for (int i = 0; i < USER_MAX; ++i) {
		_logUser(i, "login");
		//log(L_DEBUG, "{user_exists} %d like: %s", i, user_get(i)->userName);
		if (!strcmp(user_get(i)->userName, userName)) {
			return true;
		}
	}
	return false;
}

PUBLIC user_t *user_login(char* userName, char* password) {
	for (int i = 0; i < USER_MAX; ++i) {
		if (!strcmp(user_get(i)->userName, userName)) {
			if (!strcmp(user_get(i)->password, password)) {
				return user_get(i);
			} else {
				errno = E_USER_INVALID_PASSWORD;
				return NULL;
			}
		}
	}
	errno = E_USER_INVALID_USERNAME;
	return NULL;
}
