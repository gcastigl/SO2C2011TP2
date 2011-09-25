#include "../include/user.h"

void prntWelcomeMsg();

static char user[NAME_MAX_LENGTH] = {"\0"};
static int isLoggedIn = false;

void user_doLogin() {
	prntWelcomeMsg();
	char c;
	int i = 0;
	do {
		printf("\nUsername: ");
        while ((c = getchar()) != '\n' && i < NAME_MAX_LENGTH) {
        	user[i++] = c;
        	printf("%c", c);
        }
        if (user[0] == '\n') {
        	printf("\tInvalid user!");
        }
	} while(user[0] == '\n' || user[0] == '\0');
	printf("\nLogged in as: %s\n", user);
	isLoggedIn = true;
}

int user_isLoggedIn() {
	return isLoggedIn;
}

const char* user_getName() {
	if (user_isLoggedIn()) {
		return (const char*) user;
	}
	return NULL;
}

void user_logout() {
	isLoggedIn = false;
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
