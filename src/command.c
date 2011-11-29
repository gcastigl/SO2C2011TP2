#include <command.h>
#include <access/permission.h>

PRIVATE void _ls_cmd_setColor(u32int fileType);
PRIVATE char* _ls_cmd_EndingString(u32int fileType);
PRIVATE void _top_cmd_print(PROCESS** list, int* execCount, int pstatus);

// REALLY BIG FIXME: ALL this funcions should be doing a system call!!

int echo_cmd(int argc, char **argv) {
	if (argc > 0) {
		for(int i = 0; i < argc; i++) {
			printf("%s ", argv[i]);
		}
	}
	return 0;
}

int restart_cmd(int argc, char **argv) {
	_reset();		// Will neva get through this line!
	return 0xCA11AB1E;
}

int clear_cmd(int argc, char **argv) {
	shell_cleanScreen();
	return 0;
}

int help_cmd(int argc, char **argv) {
	int cmdIndex;
	if (argc == 1) {
		cmdIndex = shell_getCmdIndex(argv[0]);
		if (cmdIndex != -1) {
			tty_setFormatToCurrTTY(video_getFormattedColor(MAGENTA, BLACK));
			printf("\n%s\n", shell_getCmdsTable()[cmdIndex].help);
		} else {
			printf("\nCommand not found\n");
		}
	} else if (argc == 0) {
		int len;
		printf("\nAvailable commands:\n\n");
		cmd_table_entry* commands = shell_getCmdsTable();
		for(int i = 0; commands[i].func != NULL; i++) {
			len = strlen(commands[i].name);
			if (i%2 == 0) {
				printf("\t%12s", commands[i].name);
				printf("|");
			} else {
				printf("\t%s\n", commands[i].name);
			}
		}
		printf("\n\nType help \"cmdName\" to see the help menu for that command.\n");
	}
	return 0;
}

int calculateCpuSpeed(int (*method)()) {
	int cpuspeed;
	int iterations = 10;
	int i = iterations;
	int total = 0;
    yield();
	printf("Calculating CPU speed, please wait...\n");
    disableTaskSwitch();
	while (i-- > 0) {
		cpuspeed = (*method)();
		total += cpuspeed / iterations;
	}
    enableTaskSwitch();
	return total;
}

int getCPUspeed_cmd(int argc, char **argv) {
	int speed;
	if (_cpuIdTest() != 1) {
		printf("error: cpuid instruction is not supported.\n");
		return false;
	}
	printf("Detected cpuid instruction...\n");
	if (_rdtscTest() > 0) {
		printf("Using rdtsc to perform measurement...\n");
		speed = calculateCpuSpeed(_tscGetCpuSpeed);
	} else if (_rdmsrTest() > 0) {
		printf("Using rdmsr to perform measurement...\n");
		speed = calculateCpuSpeed(_msrGetCpuSpeed);
	} else {
		printf("error: rdtsc or rdmsr should be available to perform this command\n");
			return false;
	}
	
	printf("Detected CPU speed: %dMHz\n", speed);
	return 0;
}


int random_cmd(int argc, char **argv) {
	printf("%d\n", random());
	return 0;
}

int logout(int argc, char **argv) {
	session_logout();
	return 0;
}

int top_cmd(int argc, char**argv) {
    int slot;
    int execCount[MAX_PROCESSES] = { 0 };
    printf("Last 100:\n");
    PROCESS** all = scheduler_getAllProcesses();
    for (int i = 0; i < 100; i++) {
        slot = last100[i];
        if (all[slot] != NULL) {
            execCount[slot]++;
        }
    }
    printf("Executions over 100\n\n[ACTIVE]\n");
    printf("User\tName\tPID\tStatus\tPriority\texecCount\n");
    _top_cmd_print(scheduler_getAllProcesses(), execCount, RUNNING);
    _top_cmd_print(scheduler_getAllProcesses(), execCount, READY);
    printf("\n[BLOCKED]\n");
    printf("User\tName\tPID\tStatus\tReason\tPriority\texecCount\n");
    _top_cmd_print(scheduler_getAllProcesses(), execCount, BLOCKED);
    return 0;
}

PRIVATE void _top_cmd_print(PROCESS** list, int* execCount, int pstatus) {
    char *status[] = {"Blocked", "Ready", "Running"};
    char *priority[] = {"No Priority", "Very Low", "Low", "Normal", "High", "Very High", "Shell High"};
    char *blockType[] = {"FIFO", "INPUT", "CHILD", "SEMAPHORE", "LOGIN", "????"};
    char *nonBlockedProcessFormat = "%5s\t%5s\t%d\t%s\t%9s\t%d\n";
    char *blockedProcessFormat =    "%5s\t%5s\t%d\t%s\t%5s\t%9s\t%d\n";

	for (int i = 0; i < MAX_PROCESSES; i++) {
		PROCESS* p = list[i];
		if (p->status == pstatus) {
		    if (p->status == BLOCKED) {
                printf(blockedProcessFormat,
                        user_getName(p->ownerUid),
                        p->name,
                        p->pid,
                        status[p->status],
                        (0 <= p->waitingFlags && p->waitingFlags <= 4) ? blockType[p->waitingFlags] : blockType[5],
                        priority[p->priority % 10],
                        execCount[i]);
		    } else {
		        printf(nonBlockedProcessFormat,
		                user_getName(p->ownerUid),
		                p->name,
		                p->pid,
		                status[p->status],
		                priority[p->priority % 10],
		                execCount[i]);
		    }
		}
	}
}

int kill_cmd(int argc, char**argv) {
    if (argc == 1) {
    	int pid = atoi(argv[0]);
    	PROCESS* p = scheduler_getProcess(pid);
    	char* err = NULL;
    	if (p == NULL) {
    		err = "Process does not exist";
    	} else if (!permission_user_isOwner(p->ownerUid)) {
    		err = "Access denied";
    	}
    	if (err == NULL) {	// No error
			kill(pid);
    	} else {
    		printf("Could not kill %d: %s\n", pid, err);
    	}
    } else {
        printf("Usage:\nkill PID\n");
    }
    return 0;
}

// Permissions
int shell_useradd(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: useradd USERNAME PASSWORD\n");
		return -1;
	}
	_SysCall(SYSTEM_USERADD, argv[0], argv[1]);
	return 0;
}

int shell_userdel(int argc, char **argv) {
	if (argc != 1) {
		printf("usage: userdel USERNAME\n");
		return -1;
	}
	_SysCall(SYSTEM_USERDEL, argv[0]);
	return 0;
}

int shell_userlist(int argc, char **argv) {
	if (argc != 0) {
		printf("usage: userlist\n");
		return -1;
	}
	calluser_t userlist[USER_MAX];
	_SysCall(SYSTEM_USERLIST, userlist);
	printf("\tuid\tgid\tusername\n");
	printf("\t---\t---\t--------\n");
	for (int i = 0; i < USER_MAX; ++i) {
		calluser_t user = userlist[i];
		if (user.uid != NO_USER) {
			printf("\t%d\t%d\t%s\n", user.uid, user.gid, user.userName);
		}
	}
	return 0;
}

int shell_usersetgid(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: usersetgid USERNAME GID\n");
		return -1;
	}
	_SysCall(SYSTEM_USERSETGID, argv[0], atoi(argv[1]));
	return 0;
}

/** GROUP **/
int shell_groupadd(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: groupadd GROUP PASSWORD\n");
		return -1;
	}
	_SysCall(SYSTEM_GROUPADD, argv[0], argv[1]);
	return 0;
}

int shell_groupdel(int argc, char **argv) {
	if (argc != 1) {
		printf("usage: groupdel USERNAME\n");
		return -1;
	}
	_SysCall(SYSTEM_GROUPDEL, argv[0]);
	return 0;
}

int shell_grouplist(int argc, char **argv) {
	if (argc != 0) {
		printf("usage: grouplist\n");
		return -1;
	}
	callgroup_t grouplist[GROUP_MAX];
	_SysCall(SYSTEM_GROUPLIST, grouplist);
	printf("\tgid\tgroupname\n");
	printf("\t---\t---------\n");
	int i;
	for (i = 0; i < GROUP_MAX; ++i) {
		callgroup_t group = grouplist[i];
		if (group.gid != NO_GROUP) {
			printf("\t%d\t%s\n", group.gid, group.groupName);
		}
	}
	return 0;
}


// FIXME: idk how to keep track of the current path on the tty
int cd_cmd(int argc, char **argv) {
    if (argc == 1) {
        TTY* tty = tty_getCurrentTTY();
        u32int currentiNode = tty->currDirectory;
        fs_node_t current;
        fs_getFsNode(&current, currentiNode);
        fs_node_t *node = finddir_fs(&current, argv[0]);
        if (node != NULL) {
        	if (FILE_TYPE(node->mask) == FS_SYMLINK) {
        		char name[MAX_NAME_LENGTH];
        		read_fs(node, 0, MAX_NAME_LENGTH, (u8int*) name);
        		char* n = name;
        		return cd_cmd(1, (char**) &n);;
        	}
            if (!permission_file_hasAccess(node, R_BIT)) {
                printf("cd: You don't have read access to %s\n", argv[0]);
                return -1;
            }
            if (FILE_TYPE(node->mask) == FS_DIRECTORY) {
                tty->currDirectory = node->inode;
                memcpy(tty->currPath, node->name, strlen(node->name) + 1);
            } else {
                printf("cd: %s is not a directory\n", argv[0]);
            }
            kfree(node);
        } else {
            printf("cd: The directory \"%s\" does not exist\n", argv[0]);
        }
    }
    return 0;
}

int ls_cmd(int argc, char **argv) {
    boolean showHidden = false;
    int i = 2;
    if (argc == 1 && strcmp(argv[0], "-a") == 0) {
        showHidden = true;
        i = 0;
    }
    fs_node_t current, *node;
    char perm[MASK_STRING_LEN];
    fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
    while ((node = readdir_fs(&current, i)) != NULL) {                 // get directory i
        tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));
        if (node->name[0] != '.' || (node->name[0] == '.' && showHidden)) {
            if (showHidden) {
                if (i == 0) strcpy(node->name, "."); else if (i == 1) strcpy(node->name, "..");
            }
            mask_string(node->mask, perm);
            printf("%s\t%5s\t%5s",
                perm,
                user_getName(node->uid),
                group_getName(node->gid));
            _ls_cmd_setColor(FILE_TYPE(node->mask));
            printf("\t%s%s\n",
                node->name,
                _ls_cmd_EndingString(FILE_TYPE(node->mask))
            );
        }
        i++;
    }
    return 0;
}

PRIVATE char* _ls_cmd_EndingString(u32int fileType) {
	// log(L_DEBUG, "%x - %x", fileType, FS_DIRECTORY);
	if (fileType == FS_DIRECTORY) {
		return "/";
	}
	if (fileType == FS_SYMLINK) {
		return "@";
	}
	if (fileType == FS_PIPE) {
		return "|";
	}
	return "";
}

PRIVATE void _ls_cmd_setColor(u32int fileType) {
	switch(fileType) {
	case FS_DIRECTORY:
		tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));
		break;
	case FS_SYMLINK:
		tty_setFormatToCurrTTY(video_getFormattedColor(CYAN, BLACK));
		break;
	case FS_PIPE:
		tty_setFormatToCurrTTY(video_getFormattedColor(BROWN, BLACK));
		break;
	default:
		tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_GREY, BLACK));
	}
}

int mkdir_cmd(int argc, char **argv) {
    if(argc == 0 ) {
        printf("mkdir: missing operand\n");
    } else {
        errno = 0;
        fs_node_t current;
        fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
        createdir_fs(&current, argv[0], FS_DIRECTORY);
        char* err = NULL;
        switch(errno) {
            case OK:
                break;
            case E_ACCESS:
                err = "No write permission.";
                break;
            case E_FILE_EXISTS:
                err = "Directory exists";
                break;
            default:
                err = "Unknown error";
        }
        if (err != NULL) {
            printf("mkdir: cannot create dir %s: %s\n", argv[0], err);
        }
    }
    return 0;
}

int rm_cmd(int argc, char **argv) {
	if (argc == 1) {
        u32int currentiNode = tty_getCurrentTTY()->currDirectory;
        fs_node_t current;
        fs_getFsNode(&current, currentiNode);
        fs_node_t *node = finddir_fs(&current, argv[0]);
        char* err = NULL;
        if (node == NULL) {
        	err = "No such file or directory";
        } else if (!permission_file_hasAccess(node, W_BIT)){
        	err = "Don't have write access";
        }
        int removed = 0;
        if (err == NULL) {
            removed = removedir_fs(&current, node->inode);
        }
		switch(removed) {
		case 0:
		       // OK
		    break;
		case E_FILE_NOT_EXISTS:
		    err = "No such file or directory";
		    break;
		case E_ACCESS:
		    err = "No such file or directory";
		    break;
		}
        if (err != NULL) {
            printf("rm: cannot remove \"%s\": %s\n", argv[0], err);
            return -1;
        }
        	log(L_DEBUG, "rm: remove file returned: %d", removed);
		kfree(node);
	}
	return 0;
}

int pwd_cmd(int argc, char **argv) {
    printf("(NOT IMPLEMENTED YET) - %s\n", tty_getCurrentTTY()->currPath);
    return 0;
}

int touch_cmd(int argc, char **argv) {
    if(argc == 0 ) {
        printf("touch: missing operand\n");
    } else {
        errno = 0;
        fs_node_t current;
		fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
        createdir_fs(&current, argv[0], FS_FILE);
        char* err = NULL;
        switch(errno) {
            case OK:
                break;
            case E_ACCESS:
                err = "No write permission";
                break;
            case E_FILE_EXISTS:
                err = "File exists";
                break;
            default:
            	printf("errno: %d", errno);
                err = "Unknown error";
                break;
        }
        if (errno != 0) {
            printf("touch: cannot create file %s: %s\n", argv[0], err);
            return -1;
        }
        if (argc == 2) {
            TTY* tty = tty_getCurrentTTY();
            u32int currentiNode = tty->currDirectory;
            fs_node_t current;
            fs_getFsNode(&current, currentiNode);
            fs_node_t* file = finddir_fs(&current, argv[0]);
            write_fs(file, 0, strlen(argv[1]) + 1, (u8int*) argv[1]);
            kfree(file);
        }
    }
    return 0;
}

int ln_cmd(int argc, char **argv) {
    if (argc != 2) {
        printf("ln: missing file operand\n");
        return 0;
    }
    u32int currentiNode = tty_getCurrentTTY()->currDirectory;
    fs_node_t currentFsNode;
    fs_getFsNode(&currentFsNode, currentiNode);
    fs_node_t *target = finddir_fs(&currentFsNode, argv[0]);
    if (target == NULL) {
        printf("ln: accessing \"%s\": No such file or directory\n", argv[0]);
        return -1;
    }
    // create sym link
    u32int symLinkiNode = createdir_fs(&currentFsNode, argv[1], FS_SYMLINK);
    if (symLinkiNode == E_FILE_EXISTS) {
        printf("ln: accessing \"%s\": File exists\n", argv[1]);
        return -1;
    }
    fs_node_t symLink;
    fs_getFsNode(&symLink, symLinkiNode);
    // add contents to sym link
    //write_fs(&symLink, 0, sizeof(u32int), (u8int*) &target->inode);
    write_fs(&symLink, 0, MAX_NAME_LENGTH, (u8int*) target->name);
    kfree(target);
    return 0;
}

int cat_cmd(int argc, char **argv) {
    if (argc == 1) {
        fs_node_t current;
        fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
        fs_node_t* file = finddir_fs(&current, argv[0]);
        char* err = NULL;
        if (file == NULL) {
            err = "No such file or directory";
        } else if (FILE_TYPE(file->mask) == FS_DIRECTORY) {
            err = "Is a directory";
        }
        if (err != NULL) {
            printf("cat: %s: %s\n", argv[0], err);
            return 0;
        }
        if (!permission_file_hasAccess(file, R_BIT)) {
            printf("cat: You don't have read access to %s", argv[0]);
            return -1;
        }
        if (FILE_TYPE(file->mask) == FS_SYMLINK) {
        	char name[MAX_NAME_LENGTH];
        	read_fs(file, 0, MAX_NAME_LENGTH, (u8int*) name);
        	char* n = name;
        	return cat_cmd(1, &n);
        }
        u8int buff[512];
        int offset = 0;
        int read;
        while((read = read_fs(file, offset, 512, buff)) != 0) {
            printf("%s\n", buff);
            offset += read;
        }
        kfree(file);
    }
    return 0;
}

int mkfifo_cmd(int argc, char **argv) {
	if (argc == 1) {
		mkfifo(argv[0], O_RDWR | O_CREAT);
	}
	return 0;
}

int chmod_cmd(int argc, char **argv) {
    if (argc == 2) {
        TTY* tty = tty_getCurrentTTY();
        u32int currentiNode = tty->currDirectory;
        fs_node_t current;
        fs_getFsNode(&current, currentiNode);
        fs_node_t* file = finddir_fs(&current, argv[1]);
        if (file == NULL) {
            printf("chmod: No such file or directory %s.\n", argv[1]);
            return -1;
        }
        if (!permission_file_isOwner(file)) {
            printf("chmod: You are not the owner of %s.\n", argv[1]);
            return -2;
        }
        errno = OK;
        fs_setFileMode(file->inode, htoi(argv[0]));
        if (errno != OK) {
            printf ("ERROR %d", errno);
        }
        kfree(file);
    } else {
        printf("Usage: chmod OCTALMODE FILE");
    }
    return 0;
}

int chown_cmd(int argc, char **argv) {
    if (argc == 2) {
        TTY* tty = tty_getCurrentTTY();
        u32int currentiNode = tty->currDirectory;
        fs_node_t current;
        fs_getFsNode(&current, currentiNode);
        fs_node_t* file = finddir_fs(&current, argv[1]);
        if (file == NULL) {
            printf("chown: No such file or directory %s.\n", argv[1]);
            kfree(file);
            return -1;
        }
        if (!permission_file_isOwner(file)) {
            printf("chown: You are not the owner of %s.\n", argv[1]);
            kfree(file);
            return -2;
        }
        errno = OK;
        int uid = user_find(argv[0]);
        if (uid == NO_USER) {
            printf("chown: User %s does not exist.\n", argv[0]);
            kfree(file);
            return -2;
        } else {
            fs_setFileUid(file->inode, uid);
            if (errno != OK) {
                printf ("ERROR %d", errno);
            }
        }
        kfree(file);
    } else {
        printf("Usage: chmod OCTALMODE FILE");
    }
    return 0;
}

int chgrp_cmd(int argc, char **argv) {
    if (argc == 2) {
        TTY* tty = tty_getCurrentTTY();
        u32int currentiNode = tty->currDirectory;
        fs_node_t current;
        fs_getFsNode(&current, currentiNode);
        fs_node_t* file = finddir_fs(&current, argv[1]);
        if (file == NULL) {
            printf("chown: No such file or directory %s.\n", argv[1]);
            kfree(file);
            return -1;
        }
        if (!permission_file_isOwner(file)) {
            printf("chown: You are not the owner of %s.\n", argv[1]);
            kfree(file);
            return -2;
        }
        errno = OK;
        int gid = group_find(argv[0]);
        if (gid == NO_GROUP) {
            printf("chown: Group %s does not exist.\n", argv[0]);
            kfree(file);
            return -2;
        } else {
            fs_setFileGid(file->inode, gid);
            if (errno != OK) {
                printf ("ERROR %d", errno);
            }
        }
        kfree(file);
    } else {
        printf("Usage: chmod OCTALMODE FILE");
    }
    return 0;
}

int cacheStatus_cmd(int argc, char **argv) {
	cachedSector cache;
	int disk;
	printf("Index\tDisk\tSector\tCountDirty\t\n");
	for(int i = 0; i < CACHE_SIZE; i++) {
		cache_get(i, &cache);
		disk = cache.disk == ATA0 ? 0 : 1;
		printf("\t%d\t%d\t\t%d\t\t%d\t\t%d\n", i, disk, cache.sector, cache.accessCount, cache.dirty);
	}
	// FIXME: because printf has no option to limit the number of digits of a float,
	// a horrible extra validation was added to simulate it.
	printf("Memory usage: %d%s Kb\n\n", (SECTOR_SIZE * CACHE_SIZE) / 1024, (CACHE_SIZE%2 == 0) ? "" : ",5");
	return 0;
}

int pfiles(int argc, char **argv) {
	PROCESS* p;
	if (argc == 1) {
		p = scheduler_getProcess(atoi(argv[0]));
	} else {
		p = scheduler_getCurrentProcess();
	}
	printf("Files opened by process: %s (PID: %d)\n", p->name, p->pid);
	printf("inode\t|\tmode\n");
	boolean hasOpenedFiles = false;
	for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
		if (p->fd_table[i].mask != 0) {
			printf("%d\t\t|\t%x\n", p->fd_table[i].inode, p->fd_table[i].mode);
			hasOpenedFiles = true;
		}
	}
	if (!hasOpenedFiles) {
		printf("[none]\n");
	}
	printf("\n");
	return 0;
}

int cp_cmd(int argc, char **argv) {
	if (argc == 2) {
		char* source = argv[0];
		char* dest = argv[1];
		fs_node_t current, *sourceNode;
		fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
		sourceNode = finddir_fs(&current, source);
		if (sourceNode != NULL) {
		    char* err = NULL;
		    errno = 0;
		    fs_clone(&current, sourceNode, dest);
		    switch(errno) {
                case 0:
                    // OK
                    break;
                case E_FILE_EXISTS:
                    err = "file already exists";
                    break;
                case E_FILE_IS_DIR:
                    err = "File is a directory! (use -r)";
                    break;
                default:
                    err = "no se que paso!";
                    log(L_ERROR, "no se q paso: %d", errno);
		    }
		    if (err != NULL) {
		        printf("cp: could not copy %s: %s\n", source, err);
		        return -1;
		    }
		    return 0;
		} else {
			printf("cp: Source file: %s does not exists\n", source);
		}
	} else {
	    tty_setFormatToCurrTTY(video_getFormattedColor(RED, BLACK));
	    printf("cp: This feature does not come with Gat O.S. free trial edition.\n");
	    tty_setFormatToCurrTTY(video_getFormattedColor(MAGENTA, BLACK));
	    printf("\t\t\tConsider Purchasing it - Ask for teacher discounts!!\n");
	}
	return -1;
}

int mv_cmd(int argc, char **argv) {
	if (argc == 2) {
		int copied = cp_cmd(2, argv);
		if (copied == 0) {
			rm_cmd(1, &argv[0]);
		}
	} else {
	    cp_cmd(4, NULL);
	}
	return 0;
}

int nice_cmd(int argc, char **argv) {
    char *priorityName[] = {"No priority", "Very Low", "Low", "Normal", "High", "Very High", "Shell High", "Sky High"};
    if (argc == 0) {
        char* pname = priorityName[scheduler_getCurrentProcess()->priority];
        printf("Current priority: %s\n", pname);
        return 0;
    } else if (argc == 2) {
        int pid = atoi(argv[0]);
        int priority = atoi(argv[1]);
        if (0 <= priority && priority < 6) {
            PROCESS* p = scheduler_getProcess(pid);
            if (p != NULL) {
                if (setPriority(p->pid, priority) != -1) {
                    char* pname = priorityName[priority];
                    printf("nice: %s now has priority: %s\n", p->name, pname);
                } else {
                    printf("nice: Error: Could not set priority\n");
                }
            } else {
                printf("nice: Could not process with pid: %d", pid);
                return -1;
            }
            return 0;
        } else {
            printf("nice: Invalid priority %d. Priority must be between 0 and 6\n", priority);
            return -1;
        }
    } else {
        printf("nice: Run COMMAND with an adjusted niceness, which affects process scheduling\n");
    }
    return -1;
}
