#include <command.h>
extern PROCESS process[];

int echo_cmd(int argc, char **argv) {
	int i;
	if (argc > 0) {
		for( i = 0; i < argc; i++) {
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
		int i;
		printf("\nAvailable commands:\n\n");
		for( i=0; shell_getCmdsTable()[i].func != NULL; i++) {
			printf("\t%s\n", shell_getCmdsTable()[i].name);
		}
		printf("\nType in help \"cmdName\" to see the help menu for that \
command.\n");
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
		printf("error: rdtsc or rdmsr should be available to \
			perform this command\n");
			return false;
	}
	
	printf("Detected CPU speed: %dMHz\n", speed);
	return 0;
}


int random_cmd(int argc, char **argv) {
	int rand = random();
	printf("%d\n", rand);
	return rand;
}

int setAppearance_cmd(int argc, char **argv) {
	if (argc != 2) {
		printf("You need to call this function with 2 colors\n");
	} else {
		char* foreGround = argv[0];
		char* backGround = argv[1];
		
		int fg = parseHexa(foreGround[0]);
		int bg = parseHexa(backGround[0]);
		if (strlen(foreGround) != 1 || strlen(backGround) != 1 || fg == -1
			|| bg == -1) {
			printf("Both arguments must be a hexadecimal number between 0 \
				and F\n");
		}
		tty_setFormatToCurrTTY(video_getFormattedColor(fg, bg));
	}
	return 0;
}

int getchar_cmd(int argc, char **argv) {
	printf("Please type in a character\n");
	char c = getchar();
	printf("You pressed: %c\n", c);
	return 0;
}

int printf_cmd(int argc, char **argv) {
	printf("testing printf...\n\n");
	printf("Printing a double: %f\n", 123.456789);
	printf("Printing an integer: %d\n", 99);
	printf("Printing an unsigned integer: %u\n", 312312);
	printf("Printing a string %s\n", "This is a real cool string!");
	printf("Printing in lowercase hexadecimal notation: %x\n", 0x55fa);
	printf("Printing in uppercase hexadecimal notation: %X\n", 0x55fa);
	printf("Printing a single char: %c\n", 'c');
	return 0;
}

int scanf_cmd(int argc, char **argv) {
	int n;
	char vec[50];
	float f;
	printf("Please type in a number: ");
	scanf("%d", &n);
	printf("scanf said you typed in: %d\n\n", n);
	
	printf("An other one...Please type in some short text: ");
	scanf("%s", vec);
	printf("scanf said you typed in: %s\n\n", vec);
	
	printf("One more...Please type in a single character: ");
	scanf("%c", vec);
	printf("\nscanf said you typed in: %c\n\n", vec[0]);
	
	printf("One more...Please type in a double: ");
	scanf("%f", &f);
	printf("\nscanf said you typed in: %f\n\n", f);
	return 0;
}

int logout(int argc, char **argv) {
	session_logout();
	return 0;
}

//Processes

int idle_p(int argc, char **argv) {
    while(1) {}
    return 0;
}

int tty_p(int argc, char **argv) {
    int index = initTTY();
    log(L_DEBUG, "Created tty %d", index);
    while(1) {
        shell_update(index);
    }
    return 0;
}

int top_p(int argc, char**argv) {
    int i;
    int slot;
    int execCount[MAX_PROCESSES] = { 0 };
    char *status[] = {"Ready", "Blocked", "Child Wait", "Sleeping", "Running"};
    printf("Last 100:\n");
    for (i = 0; i < 100; i++) {
        slot = last100[i];
        if (process[slot].slotStatus == OCCUPIED)
            execCount[slot]++;
    }
    printf("Name\tPID\tStatus\tExecutions over 100\n");
    for (i = 0; i < MAX_PROCESSES; i++) {
        if ((process[i].slotStatus == OCCUPIED)) {
            
            printf("%s\t%d\t%s\t%d\n", process[i].name, process[i].pid, status[process[i].status] ,execCount[i]);
        }
    }
    
    return 0;
}

int kill_p(int argc, char**argv) {
    if (argc == 1) {
        kill(atoi(argv[0]));
    } else {
        printf("Usage:\nkill PID");
    }
    return 0;
}

int eternumWhile_p(int argc, char** argv) {
    while(1);
}

// TODO: finish this funcion!
int diskManagerTest(int argc, char **argv) {
	iNode node;
	node.flags = FS_FILE;
	int inodeNumber = diskManager_nextInode();
	diskManager_createInode(&node, inodeNumber, "test.txt");
	char *contents = "The File interface represents file data typically obtained from the underlying file system, and the Blob interface (\"Binary Large Object\" -- a name originally introduced to web APIs in Google Gears) represents immutable raw data. File or Blob reads should happen asynchronously on the main thread, with an optional synchronous API used within threaded web applications. An asynchronous API for reading files prevents blocking and UI \"freezing\" on a user agent's main thread. This specification defines an asynchronous API based on an event model to read and access a File or Blob's data. A FileReader object provides asynchronous read methods to access that file's data through event handler attributes and the firing of events. The use of events and event handlers allows separate code blocks the ability to monitor the progress of the read (which is particularly useful for remote drives or mounted drives, where file access performance may vary from local drives) and error conditions that may arise during reading of a file. An example will be illustrative.";
	u32int len = strlen(contents) + 1;
	diskManager_writeContents(inodeNumber, contents, len, 0);
	int calcSize = diskManager_size(inodeNumber);
	char asd[calcSize];
	diskManager_readContents(inodeNumber, asd, calcSize, 0);
	printf("Recovered contents (%d, originalSize = %d):\n%s\n", calcSize, len, asd);
	char part[40];
	diskManager_readContents(inodeNumber, part, 40, 10);
	printf("Reading from 10 to 50: %s\n", part);
	return 0;
}



/*
int format(int argc, char **argv) {
	printf("Formatting drive ATA0...\n");
	fs_format();
	return 0;
}*/

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
	printf("\tusername\tuid\tgid\n");
	printf("\t--------\t---\t---\n");
	int i;
	for (i = 0; i < USER_MAX; ++i) {
		calluser_t user = userlist[i];
		if (user.uid != NO_USER) {
			printf("\t%s\t%d\t%d\n", user.userName, user.uid, user.gid);
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
