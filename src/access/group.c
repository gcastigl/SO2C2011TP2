#include <access/group.h>

PRIVATE group_t groups[GROUP_MAX];

PRIVATE boolean group_parse(char *line);
PRIVATE void group_reset();
PRIVATE int group_findOpenUid();
PRIVATE boolean group_add(int uid);
PRIVATE boolean group_del(int uid);
PRIVATE boolean group_isValidFormat(char *token);

PUBLIC boolean group_isSet(int uid) {
	if (0 <= uid && uid < GROUP_MAX) {
		return groups[uid].uid != NO_GROUP;
	}
	log(L_ERROR, "INVALID UID %d", uid);
	return false;
}

PRIVATE int group_findOpenUid() {
	for (int i = 0; i < GROUP_MAX; ++i) {
		if (!group_isSet(i)) {
			return i;
		}
	}
	return NO_GROUP;
}

PRIVATE boolean group_add(int uid) {
	if (!group_isSet(uid)) {
		groups[uid].uid = uid;
		groups[uid].gid = uid;
		groups[uid].groupName[0] = '\0';
		groups[uid].password[0] = '\0';
		return true;
	} else {
		return false;
	}
}

PRIVATE boolean group_del(int uid) {
	group_t *group = group_get(uid);
	if (group != NULL) {
		group->uid = NO_GROUP;
		return true;
	} else {
		return false;
	}
}

PUBLIC group_t *group_get(int uid) {
	if (group_isSet(uid)) {
		return &groups[uid];
	} else {
		return NULL;
	}
}

PUBLIC boolean do_grouplist(callgroup_t *callgroups) {
	char groupstring[128];
	for (int i = 0; i < GROUP_MAX; ++i) {
		group_string(i, groupstring);
		log(L_DEBUG, "group %d: %s", i, groupstring);
	}

	group_t *group = NULL;
	log(L_DEBUG, "dogrouplist");
	for (int i = 0; i < GROUP_MAX; ++i) {
		group = group_get(i);
		if (group == NULL) {
			callgroups[i].uid = NO_GROUP;
			callgroups[i].gid = NO_GROUP;
			strcpy(callgroups[i].groupName, "no-group");
		} else {
			callgroups[i].uid = group->uid;
			callgroups[i].gid = group->gid;
			strcpy(callgroups[i].groupName, group->groupName);
		}
		log(L_DEBUG, "callu: %d:%d, %d:%d, %s:%s",
			group->uid, callgroups[i].uid,
			group->gid, callgroups[i].gid,
			group->groupName, callgroups[i].groupName
		);
	}
	return true;
}

PUBLIC boolean do_groupsetgid(char *groupName, int gid) {
	int uid = group_find(groupName);
	group_t *group = group_get(uid);
	if (group == NULL) {
		log(L_ERROR, "invalid group uid, %d: %s", uid, groupName);
		return false;
	} else {
		log(L_INFO, "old: %d. new: %d", group->gid, gid);
		group->gid = gid;
		return true;
	}
}

PUBLIC boolean group_setGroupname(int uid, char *groupname) {
	if (!group_isSet(uid)) {
		log(L_ERROR, "invalid uid %d", uid);
		return false;
	}
	if (groupname == NULL || !strlen(groupname)) {
		log(L_ERROR, "groupname cannot be empty");
		return false;
	}
	if (!group_isValidFormat(groupname)) {
		log(L_ERROR, "groupname cannot contain the ':' character");
		return false;
	}
	if (group_find(groupname) != NO_GROUP ) {
		log(L_ERROR, "groupname %s already exists", groupname);
		return false;
	}
	strcpy(group_get(uid)->groupName, groupname);
	return true;
}

PUBLIC boolean group_setPassword(int uid, char *password) {
	if (!group_isSet(uid)) {
		log(L_ERROR, "invalid uid %d", uid);
		return false;
	}
	if (!group_isValidFormat(password)) {
		log(L_ERROR, "password cannot contain the ':' character");
		return false;
	}
	strcpy(group_get(uid)->password, password);
	return true;
}

PUBLIC boolean group_setGid(int uid, int gid) {
	if (!group_isSet(uid)) {
		log(L_ERROR, "invalid uid %d", uid);
		return false;
	}
	if (!group_isSet(gid)) {
		log(L_ERROR, "invalid gid %d", gid);
		return false;
	}
	group_get(uid)->gid = gid;
	return true;
}

PRIVATE void group_reset() {
	for (int i = 0; i < GROUP_MAX; ++i) {
		groups[i].uid = NO_GROUP;
	}
}

PUBLIC boolean group_string(int uid, char *string) {
	group_t *group = group_get(uid);
	if (group) {
		sprintf(string, "%d:%d:%s:%s",
			group->uid,
			group->gid,
			group->groupName,
			group->password
		);
		return true;
	} else {
		string[0] = '\0';
		return false;
	}
}

PUBLIC void group_init() {
	group_reset();
	group_parse("0:0:root:pass\n");
	group_parse("10:10:qcho:x\n");
	group_parse("11:11:qcho1:pass1\n");
	group_parse("12:12:qcho2:pass2");

	do_groupadd("a", "a");
	do_groupadd("guest", "");
	do_groupadd("Tres", "Cuatro");

	do_groupdel("qcho2");
	char groupstring[128];
	for (int i = 0; i < GROUP_MAX; ++i) {
		group_string(i, groupstring);
		log(L_DEBUG, "group %d: %s", i, groupstring);
	}
}

PRIVATE boolean group_parse(char *line) {
	enum { UID = 0, GID, GROUPNAME, PASSWORD} field = UID;
	boolean parsing_ok = true;
	char *p = strtok(line, ":");
	int uid = atoi(p);
	parsing_ok = group_add(atoi(p));
	while (p != NULL && parsing_ok) {
		p = strtok(NULL, ":\n");
		field++;
		switch (field) {
			case UID:
				// DONE.
				break;
			case GID:
				parsing_ok = group_setGid(uid, atoi(p));
				break;
			case GROUPNAME:
				parsing_ok = group_setGroupname(uid, p);
				break;
			case PASSWORD:
				parsing_ok = group_setPassword(uid, p);
				break;
		}
	}
	if (!parsing_ok && field != UID) {
		// delete invalid group.
		group_del(uid);
	}
	return parsing_ok;
}

PUBLIC int group_find(char *groupName) {
	for (int i = 0; i < GROUP_MAX; ++i) {
		if (!strcmp(group_get(i)->groupName, groupName)) {
			return i;
		}
	}
	return NO_GROUP;
}

PUBLIC group_t *group_login(int uid, char* password) {
	group_t *group = group_get(uid);
	if (group != NULL) {
		if (!strcmp(group->password, password)) {
			return group;
		} else {
			errno = E_GROUP_INVALID_PASSWORD;
			return NULL;
		}
	} else {
		errno = E_GROUP_INVALID_GROUPNAME;
		return NULL;
	}
}

PRIVATE boolean group_isValidFormat(char *token) {
	for (int i = 0; i < strlen(token); ++i) {
		if (token[i] == ':') {
			return false;
		}
	}
	return true;
}

PUBLIC boolean do_groupadd(char *groupName, char *password) {
	boolean addOk = true;
	int uid = group_findOpenUid();
	group_add(uid);
	addOk = group_setGroupname(uid, groupName);
	addOk = group_setPassword(uid, password);
	if (!addOk) {
		group_del(uid);
	}
	return addOk;
}

PUBLIC boolean do_groupdel(char *groupName) {
	return group_del(group_find(groupName));
}
