#include "tty.h"

static TTY tty[NUMBER_OF_TTYs];
static int currTTY = 0;

void initTTYs() {
    int i;
    for(i = 0; i < NUMBER_OF_TTYs; i++) {
        
    }
}

void initTTY(int index) {
    tty[index].screen = malloc(2 * ROWS * COLUMNS);
    tty[index].offset = 0;
    tty[index].buffer.head = 0;
    tty[index].buffer.tail = 0;
}
