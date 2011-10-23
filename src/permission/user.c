#include <permission/user.h>

PRIVATE user_t *users[USER_MAX];

PRIVATE user_t *_parseUser(char* line);
PRIVATE void _resetUsers();
PRIVATE int _findOpenUid();
PRIVATE boolean user_add(user_t *user);
PRIVATE boolean user_remove(user_t *user);


PRIVATE boolean user_add(user_t *user) {
	if (user->uid < 0 || user->uid >= USER_MAX) {
		log(L_ERROR, "{user_add} invalid uid: %d (%s)", user->uid, user->userName);
		return false;
	}
	if (user_find(user->userName) >= 0) {
		log(L_ERROR, "{user_add} user with name %s already exists", user_get(user->uid)->userName);
		return false;
	}
	if (user_get(user->uid) != NULL) {
		log(L_ERROR, "{user_add} uid %d already set: %s", user->uid, user_get(user->uid)->userName);
		return false;
	}
	users[user->uid] = user;
	return true;
}

PRIVATE boolean user_remove(user_t *user) {
	if (user == NULL) {
		log(L_ERROR, "invalid user to remove");
		return false;
	}
	users[user->uid] = NULL;
	return true;
}

PRIVATE calluser_t *_userToCallUser(user_t *user) {
	if (user == NULL) {
		return NULL;
	}
	log(L_DEBUG, "1");
	calluser_t *calluser = kmalloc(sizeof(calluser_t));
	log(L_DEBUG, "2");
	calluser->uid = user->uid;
	log(L_DEBUG, "3");
	calluser->gid = user->gid;
	log(L_DEBUG, "4 %d-%d-%s", user->uid, user->gid, user->userName);
	calluser->userName = kmalloc(strlen(user->userName)*sizeof(char));
	log(L_DEBUG, "5");
	strcpy(calluser->userName, user->userName);
	log(L_DEBUG, "6");
	return calluser;
}

PUBLIC boolean do_userlist(calluser_t **callusers) {
	log(L_DEBUG, "douserlist");
	for (int i = 0; i < USER_MAX; ++i) {
		callusers[i] = _userToCallUser(user_get(i));
		if (callusers[i] != NULL) {
			log(L_DEBUG, "callu: %d, %d, %s",
				callusers[i]->uid,
				callusers[i]->gid,
				callusers[i]->userName
			);
		}
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
		if (gid < 0 || gid >= USER_MAX) {
			log(L_ERROR, "invalid gid: %d", gid);
			return false;
		}
		log(L_INFO, "old: %d. new: %d", user->gid, gid);
		user->gid = gid;
		return true;
	}
}

PUBLIC boolean do_useradd(char *userName, char *password) {
	user_t *user = (user_t *)kmalloc(sizeof(user_t));
	if (password == NULL || !strlen(userName)) {
		log(L_ERROR, "not username provided.");
		return false;
	}
	user->userName = kmalloc(strlen(userName)*sizeof(char));
	strcpy(user->userName, userName);
	if (password != NULL && strlen(password)) {
		user->password = kmalloc(strlen(password)*sizeof(char));
		strcpy(user->password, password);
	} else {
		user->password = "";
	}
	int uid = _findOpenUid();
	user->uid = uid;
	user->gid = uid;
	user->homePath = kmalloc(256*sizeof(char));
	user->userInfo = "";
	sprintf(user->homePath, "/home/%s", userName);
	user_add(user);
	return true;
}

PUBLIC boolean do_userdel(char *userName) {
	int uid = user_find(userName);
	user_t *user = user_get(uid);
	if (user == NULL) {
		log(L_ERROR, "invalid user delete, %d", uid);
		return false;
	} else {
		user_remove(user);
		return true;
	}
}

PRIVATE int _findOpenUid() {
	for (int i = 0; i < USER_MAX; ++i) {
		if (user_get(i) == NULL) {
			return i;
		}
	}
	return -1;
}

PRIVATE void _resetUsers() {
	for (int i = 0; i < USER_MAX; ++i) {
		users[i] = NULL;
	}
}

PUBLIC char *user_toString(user_t *user) {
	if (user == NULL) {
		return "no-user";
	}
	char *string = kmalloc(100*sizeof(char));
	sprintf(string, "%s:%s:%d:%d:%s:%s",
		user->userName,
		user->password,
		user->uid,
		user->gid,
		user->userInfo,
		user->homePath
	);
	return string;
}

PUBLIC void user_init() {
	_resetUsers();
	user_add(_parseUser("qcho3:x:12:10:Qcho:/home/qcho"));
	user_add(_parseUser("root:x:0:0:root:/root"));
	user_add(_parseUser("qcho:x:10:10:Qcho:/home/qcho"));
	user_add(_parseUser("qcho2:x:11:10:Qcho:/home/qcho"));

	do_useradd("Horacio", "Gomez");
	do_useradd("Dos", "");
	do_useradd("Tres", "Cuatro");

	do_userdel("qcho2");
	for (int i = 0; i < USER_MAX; ++i) {
		log(L_DEBUG, "user %d: %s", i, user_toString(user_get(i)));
	}
}

PRIVATE user_t *_parseUser(char* line) {
	user_t *user = (user_t *)kmalloc(sizeof(user_t));
	enum { USERNAME = 0, PASSWORD, UID, GID, USERINFO, HOMEPATH} item;
	int i = 0;
	char *buff = kmalloc(64 * sizeof(char));
	int start = 0;
	int len;
	char c;
	item = USERNAME;
	do {
		c = line[i++];
		if (c == ':' || c == '\0') {
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
				default:
					return user;
			}
			start = i;
			item++;
		}
	} while (c != '\0');
	kfree(buff);
    return user;
}

/**
 * returns the user or NULL if not exist.
 */
PUBLIC user_t *user_get(int uid) {
	if (uid < 0 || uid >= USER_MAX) {
		log(L_ERROR, "invalid uid: %d", uid);
		return NULL;
	}
	user_t *user = users[uid];
	if (user == NULL) {
		log(L_TRACE, "invalid user uid: %d", uid);
	}
	log(L_DEBUG, "uid %d: %d", uid, (int)user);
	return user;
}

PUBLIC int user_find(char *userName) {
	for (int i = 0; i < USER_MAX; ++i) {
		if (!strcmp(user_get(i)->userName, userName)) {
			return i;
		}
	}
	return -1;
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
