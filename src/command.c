#include "../include/command.h"


//Console commands
int echo_cmd(int argc, char *argv[]) {
	int i;
	if (argc > 0) {
		for( i = 0; i < argc; i++) {
			printf("%s ", argv[i]);
		}
	}
	return 0;
}

int restart_cmd(int argc, char *argv[]) {
	_reset();		// Will neva get through this line! =P
	return 0xCA11AB1E;
}

int clear_cmd(int argc, char *argv[]) {
	video_clearScreen();
	return 0;
}

int help_cmd(int argc, char *argv[]) {
	int cmdIndex;
	if (argc == 1) {
		cmdIndex = getCmdIndex(argv[0]);
		if (cmdIndex != -1) {
			printf("\n%s\n", getCmdsTable()[cmdIndex].help);
		} else {
			printf("\nCommand not found\n");
		}
	} else if (argc == 0) {
		int i;
		printf("\nAvailable commands:\n\n");
		for( i=0; getCmdsTable()[i].func != NULL; i++) {
			printf("\t%s\n", getCmdsTable()[i].name);
		}
		printf("\nType in help \"cmdName\" to see the help menu for that \
command.\n");
	}
}

int calculateCpuSpeed(int (*method)()) {
	int cpuspeed;
	int iterations = 10;
	int i = iterations;
	int total = 0;
	printf("Calculating CPU speed, please wait...\n");
	while (i-- > 0) {
		cpuspeed = (*method)();
		total += cpuspeed / iterations;
	}
	return total;
}

int getCPUspeed_cmd(int argc, char *argv[]) {
	int speed;
	if (_cpuIdTest() != 1) {
		printf("error: cpuid instruction is not supported.\n");
		return FALSE;
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
			return FALSE;
	}
	
	printf("Detected CPU speed: %iMHz\n", speed);
	return 0;
}


int random_cmd(int argc, char *argv[]) {
	int rand = random();
	printf("%d", rand);
	return rand;
}

int setAppearance_cmd(int argc, char *argv[]) {
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
		setVideoColor(bg, fg);
	}
	return 0;
}

int getchar_cmd(int argc, char *argv[]) {
	printf("Please type in a character\n");
	char c = getchar();
	printf("You pressed: %c\n", c);
	return 0;
}

int printf_cmd(int argc, char *argv[]) {
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

int scanf_cmd(int argc, char *argv[]) {
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

//Processes
int idle_process(int argc, char** argv) {
    while(1) {
        printf("Idle!");
    }
}

int shell_process(int argc, char** argv) {
    initShell();
    while(1) {
        updateShell();
    }
}