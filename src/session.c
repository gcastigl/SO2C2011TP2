#include <session.h>

void prntWelcomeMsg();
int isValidUser(char* userName);

static char user[NAME_MAX_LENGTH] = {"\0"};
static int isLoggedIn = false;

void session_login() {
	TTY* tty = tty_getCurrentTTY();
	tty_clean(tty);
	prntWelcomeMsg();
	/*int validUser;
	do {
		printf("\nUsername: ");
		gets(user);
		validUser = isValidUser(user);
        if (!validUser) {
        	printf("\tInvalid username!\n");
        }
	} while(!validUser);
	printf("Password: ********\n");
	printf("\nLogged in as: %s\n", user);*/
	isLoggedIn = true;
	strcpy(user, "admin");
}

int session_isLoggedIn() {
	return isLoggedIn;
}

const char* session_getName() {
	if (session_isLoggedIn()) {
		return (const char*) user;
	}
	return NULL;
}

void session_logout() {
	isLoggedIn = false;
}

int isValidUser(char* userName) {
	return strcmp(userName, "admin") == 0 || strcmp(userName, "guest") == 0;
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
