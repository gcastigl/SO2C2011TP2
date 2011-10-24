#include <tty.h>

PRIVATE TTY tty[MAX_TTYs];
int currentTTY = 0;
PUBLIC int activeTTYs = 0;
extern void shell_update(int index);
void startTTYs() {
    char name[5];
    for (int i = 1; i <= MAX_TTYs; i++) {
        sprintf(name, "tty%d", i);
        createProcess(name, &tty_p, 0, NULL, DEFAULT_STACK_SIZE, &clean, i, BACKGROUND, READY, ((i == 1) ? SHELL_HIGH : VERY_LOW));
    }
}

int initTTY(int pid) {
    int index = activeTTYs++;
    tty[index].id = index;
	tty[index].terminal = (char*) kmalloc(TOTAL_VIDEO_SIZE);
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
	
    return index;
}

void tty_setCurrent(int tty) {
    TTY* currTTY = tty_getCurrentTTY();
    setPriority(currTTY->pid, VERY_LOW);
	currentTTY = tty;
	currTTY = tty_getCurrentTTY();
    setPriority(currTTY->pid, SHELL_HIGH);
	video_clearScreen(video_getFormattedColor(currTTY->fgColor, currTTY->bgColor));
	video_setOffset(0);
	video_write(currTTY->terminal, currTTY->offset);
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

void tty_write(TTY* tty, char* buffer, u32int size) {
	int j;
	char format = video_getFormattedColor(tty->fgColor, tty->bgColor);
	for (j = 0; j < size; ++j) {
		if (tty->offset >= TOTAL_VIDEO_SIZE) {
			terminal_scroll(tty->terminal);
			tty->offset = terminal_getOffset(ROWS - 1, 0);
			video_clearRow(ROWS - 1, -1);
		}
		int newOffset = terminal_prtSpecialCharater(tty->terminal, tty->offset, buffer[j], format);
		tty->offset += newOffset;
		if (newOffset != 0) { // It was an special character, nothing to print
			continue;
		}
		tty->terminal[tty->offset++] = buffer[j];
		tty->terminal[tty->offset++] = format;
	}
}

void tty_clean(TTY* tty) {
	char format = video_getFormattedColor(tty->fgColor, tty->bgColor);
	for(int i = 0; i < TOTAL_VIDEO_SIZE; i+=2) {
		tty->terminal[i] = ' ';
		tty->terminal[i + 1] = format;
	};
	tty->offset = 0;
	video_clearScreen(video_getFormattedColor(tty->fgColor, tty->bgColor));
	video_setOffset(0);
	video_write(tty->terminal, tty->offset);
}

char tty_getCurrTTYFormat() {
	TTY* currTTY = tty_getCurrentTTY();
	return video_getFormattedColor(currTTY->fgColor, currTTY->bgColor);
}

void tty_setFormatToCurrTTY(char format) {
	TTY* currTTY = tty_getCurrentTTY();
	currTTY->fgColor = video_getFGcolor(format);
	currTTY->bgColor = video_getBGcolor(format);
}

int tty_p(int argc, char **argv) {
    int index = initTTY(getCurrentPID());
    while(1) {
        shell_update(index);
    }
    return 0;
}
