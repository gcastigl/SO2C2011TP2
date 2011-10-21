#include <session.h>

PRIVATE user_t *currentUser = NULL;
void prntWelcomeMsg();
int isValidUser(char* userName);

static int isLoggedIn = false;

PRIVATE boolean tryLogin(char * userName, char *password);

void session_login() {
	TTY* tty = tty_getCurrentTTY();
	tty_clean(tty);
	prntWelcomeMsg();
	int validUser;
	static char user[NAME_MAX_LENGTH] = {"\0"};
	do {
		printf("\nUsername (try qcho): ");
		gets(user);
		validUser = isValidUser(user);
        if (!validUser) {
        	printf("\tInvalid username!\n");
        }
	} while(!validUser);
	int validPassword;
	int retry = 3;
	static char password[PASS_MAX_LENGTH] = {"\0"};
	do {
		printf("\nPassword (try x): ");
		gets(password);
		isLoggedIn = tryLogin(user, password);
		if (!isLoggedIn) {
			printf("\tInvalid password!\n");
		}
	} while(!isLoggedIn && --retry);
	if (retry == 0) {
		printf("\tInvalid password! Please try again.\n");
	} else {
		printf("\nLogged in as: %s\n", session_getName());
	}
}

PRIVATE boolean tryLogin(char * userName, char *password) {
	currentUser = user_login(userName, password);
	return currentUser != NULL;
}

int session_isLoggedIn() {
	return isLoggedIn;
}

const char* session_getName() {
	if (session_isLoggedIn()) {
		return currentUser->userName;
	}
	return NULL;
}

void session_logout() {
	isLoggedIn = false;
}

int isValidUser(char* userName) {
	return user_exists(userName);
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
