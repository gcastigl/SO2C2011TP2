#include <access/group.h>
#include <access/permission.h>

PRIVATE group_t groups[GROUP_MAX];

PRIVATE boolean group_parse(char *line);
PRIVATE void group_reset();
PRIVATE int group_findOpenGid();
PRIVATE boolean group_add(int gid);
PRIVATE boolean group_del(int gid);
PRIVATE boolean group_isValidFormat(char *token);

PUBLIC boolean group_isSet(int gid) {
	if (0 <= gid && gid < GROUP_MAX) {
		return groups[gid].gid != NO_GROUP;
	}
	log(L_ERROR, "INVALID GID %d", gid);
	return false;
}

PRIVATE int group_findOpenGid() {
	for (int i = 0; i < GROUP_MAX; ++i) {
		if (!group_isSet(i)) {
			return i;
		}
	}
	return NO_GROUP;
}

PRIVATE boolean group_add(int gid) {
	if (!group_isSet(gid)) {
		groups[gid].gid = gid;
		groups[gid].groupName[0] = '\0';
		groups[gid].password[0] = '\0';
		return true;
	} else {
		return false;
	}
}

PRIVATE boolean group_del(int gid) {
	group_t *group = group_get(gid);
	if (group != NULL) {
		group->gid = NO_GROUP;
		return true;
	} else {
		return false;
	}
}

PUBLIC group_t *group_get(int gid) {
	if (group_isSet(gid)) {
		return &groups[gid];
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
			callgroups[i].gid = NO_GROUP;
			strcpy(callgroups[i].groupName, "no-group");
		} else {
			callgroups[i].gid = group->gid;
			strcpy(callgroups[i].groupName, group->groupName);
		}
		log(L_DEBUG, "callg: %d:%d, %s:%s",
			group->gid, callgroups[i].gid,
			group->groupName, callgroups[i].groupName
		);
	}
	return true;
}

PUBLIC boolean group_setGroupname(int gid, char *groupname) {
	if (!group_isSet(gid)) {
		log(L_ERROR, "invalid gid %d", gid);
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
	strcpy(group_get(gid)->groupName, groupname);
	return true;
}

PUBLIC boolean group_setPassword(int gid, char *password) {
	if (!group_isSet(gid)) {
		log(L_ERROR, "invalid gid %d", gid);
		return false;
	}
	if (!group_isValidFormat(password)) {
		log(L_ERROR, "password cannot contain the ':' character");
		return false;
	}
	strcpy(group_get(gid)->password, password);
	return true;
}

PRIVATE void group_reset() {
	for (int i = 0; i < GROUP_MAX; ++i) {
		groups[i].gid = NO_GROUP;
	}
}

PUBLIC boolean group_string(int gid, char *string) {
	group_t *group = group_get(gid);
	if (group) {
		sprintf(string, "%d:%s:%s",
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
	group_parse("0:root:pass\n");
	group_parse("10:qcho:x\n");
	group_parse("11:qcho1:pass1\n");
	group_parse("12:qcho2:pass2");

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
	enum { GID = 0, GROUPNAME, PASSWORD} field = GID;
	boolean parsing_ok = true;
	char *p = strtok(line, ":");
	int gid = atoi(p);
	parsing_ok = group_add(atoi(p));
	while (p != NULL && parsing_ok) {
		p = strtok(NULL, ":\n");
		field++;
		switch (field) {
			case GID:
				// DONE.
				break;
			case GROUPNAME:
				parsing_ok = group_setGroupname(gid, p);
				break;
			case PASSWORD:
				parsing_ok = group_setPassword(gid, p);
				break;
		}
	}
	if (!parsing_ok && field != GID) {
		// delete invalid group.
		group_del(gid);
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

PUBLIC group_t *group_login(int gid, char* password) {
	group_t *group = group_get(gid);
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
	int gid = group_findOpenGid();
	group_add(gid);
	addOk = group_setGroupname(gid, groupName);
	addOk = group_setPassword(gid, password);
	if (!addOk) {
		group_del(gid);
	}
	return addOk;
}

PUBLIC boolean do_groupdel(char *groupName) {


    int gid = group_find(groupName);
        if (gid == NO_GROUP) {
            printf("No group exists with %s groupname.\n", groupName);
            errno = INVALID_INPUT;
            return false;
        }
        if (session_getEgid() == gid) {
            printf("You can't delete your own group.\n");
            errno = INVALID_INPUT;
            return false;
        }
        if (permission_group_isOwner(gid)) {
            return group_del(gid);
        } else {
            printf("Access denied.\n");
            errno = EACCES;
            return false;
        }
}

PUBLIC char *group_getName(int gid) {
	group_t *group = group_get(gid);
	if (group != NULL) {
		return group->groupName;
	} else {
		return "unknown";
	}
}
