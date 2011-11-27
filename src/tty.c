#include <tty.h>
#include <util/logger.h>

extern void shell_update();
void initTTY(int pid);

PRIVATE TTY tty[MAX_TTYs];
PRIVATE int inactiveTTYpriority = VERY_LOW;
PRIVATE int activeTTYpriority = HIGH;
PRIVATE int currentTTY = 0;
PUBLIC int activeTTYs = 0;

void startTTYs() {
    char name[5];
    for (int i = 0; i < MAX_TTYs; i++) {
        sprintf(name, "tty%d", i + 1);
        int priority = ((i == 0) ? activeTTYpriority : inactiveTTYpriority);
        scheduler_schedule(name, &tty_p, 0, NULL, DEFAULT_STACK_SIZE, i, BACKGROUND, READY, priority);
    }
}

void initTTY(int pid) {
    log(L_DEBUG, "Starting tty %d", pid);
    int index = activeTTYs++;
    tty[index].id = index;
    circularBuffer_init(&tty[index].input_buffer, TTY_INPUT_BUFFER_SIZE);
    tty[index].screen = (char*) kmalloc(TOTAL_VIDEO_SIZE);
    for (int i = 0; i < TOTAL_VIDEO_SIZE; i+=2) {
        tty[index].screen[i] = 0;
        tty[index].screen[i + 1] = 0;
    }
	tty[index].offset = 0;
    tty[index].bufferOffset = 0;
	tty[index].bgColor = BLACK;
	tty[index].fgColor = WHITE;
    tty[index].pid = pid;
	fs_node_t root;
	fs_getRoot(&root);
	tty[index].currDirectory = root.inode;
	strcpy(tty[index].currPath, root.name);
	tty[index].currPathOffset = strlen(root.name);
	if (index != 0) {// Leave only the first TTY active to do the login
	    scheduler_blockCurrent(W_LOGIN);
	}
}

void tty_setCurrent(int tty) {
	_cli();
    TTY* currTTY = tty_getCurrentTTY();
    setPriority(currTTY->pid, inactiveTTYpriority);
    currentTTY = tty;
	currTTY = tty_getCurrentTTY();
    setPriority(currTTY->pid, activeTTYpriority);

    video_clearScreen(video_getFormattedColor(currTTY->fgColor, currTTY->bgColor));
	video_setOffset(0);
	video_write(currTTY->screen, currTTY->offset);
	_sti();
}

int tty_getCurrent() {
	return currentTTY;
}

TTY* tty_getTTY(int index) {
	return &tty[index];
}

TTY* tty_getCurrentTTY() {
	return &tty[currentTTY];
}

void tty_getCurrentNode(fs_node_t* node) {
	u32int currentiNode = tty_getCurrentTTY()->currDirectory;
	fs_getFsNode(node, currentiNode);
}
//FIXME: this shouold be a fs_node_t*
PUBLIC void tty_setCurrentNode(fs_node_t node) {
    tty_getCurrentTTY()->currDirectory = node.inode;
    memcpy(tty_getCurrentTTY()->currPath, node.name, strlen(node.name) + 1);
}

void tty_write(TTY* tty, char* buffer, u32int size) {
	int j;
	char format = video_getFormattedColor(tty->fgColor, tty->bgColor);
	for (j = 0; j < size; ++j) {
		if (tty->offset >= TOTAL_VIDEO_SIZE) {
			terminal_scroll(tty->screen);
			tty->offset = terminal_getOffset(ROWS - 1, 0);
			video_clearRow(ROWS - 1, -1);
		}
		int newOffset = terminal_prtSpecialCharater(tty->screen, tty->offset, buffer[j], format);
		tty->offset += newOffset;
		if (newOffset != 0) { // It was an special character, nothing to print
			continue;
		}
		tty->screen[tty->offset++] = buffer[j];
		tty->screen[tty->offset++] = format;
	}
}

boolean tty_hasInput(TTY* tty) {
	return !circularBuffer_isEmpty(&tty->input_buffer);
}

void tty_clean(TTY* tty) {
	char format = video_getFormattedColor(tty->fgColor, tty->bgColor);
	for(int i = 0; i < TOTAL_VIDEO_SIZE; i+=2) {
		tty->screen[i] = ' ';
		tty->screen[i + 1] = format;
	};
	tty->offset = 0;
	video_clearScreen(video_getFormattedColor(tty->fgColor, tty->bgColor));
	video_setOffset(0);
	video_write(tty->screen, tty->offset);
}

char tty_getCurrTTYFormat() {
	TTY* currTTY = tty_getCurrentTTY();
	return video_getFormattedColor(currTTY->fgColor, currTTY->bgColor);
}

void tty_setFormatToCurrTTY(char format) {
	tty_setFormat(tty_getCurrentTTY(), format);
}

void tty_setFormat(TTY* tty, char format) {
	tty->fgColor = video_getFGcolor(format);
	tty->bgColor = video_getBGcolor(format);
}

int tty_p(int argc, char **argv) {
    initTTY(scheduler_currentPID());
    while(1) {
        shell_update();
    }
    return 0;
}
