#include "screen.h"
#include "../system/isr.h"
#include "../system/in_out.h"
#include "timer.h"

int16_t *video_memory = (int16_t *) 0xB8000;

#define BUFFER_SIZE 1000
char array_out[BUFFER_SIZE];
buffer_t stdout;

#define ESC '\x1B'
#define BELL '\x07'

#define DEFAULT_SETTINGS 0x07

#define SCREEN_SIZE_X 80
#define SCREEN_SIZE_Y 25
uint8_t screen_state = 0; // 0=normal, 1=scaped, 2=parameters.

#define SCREEN_MAX_PARAM_COUNT 16
uint8_t screen_param_count = 0;
int screen_param[SCREEN_MAX_PARAM_COUNT];

uint8_t screen_cursor_x = 0;
uint8_t screen_cursor_y = 0;
uint8_t screen_settings = DEFAULT_SETTINGS;

#define VGA_HIGH_CURSOR_BYTE 14
#define VGA_LOW_CURSOR_BYTE 15
#define VGA_MODE_PORT 0x3D4
#define VGA_IO_PORT 0x3D5

static void update_cursor() {
    int16_t cursorLocation = screen_cursor_y * SCREEN_SIZE_X + screen_cursor_x;
    outb(VGA_MODE_PORT, VGA_HIGH_CURSOR_BYTE);
    outb(VGA_IO_PORT, cursorLocation >> 8);
    outb(VGA_MODE_PORT, VGA_LOW_CURSOR_BYTE);
    outb(VGA_IO_PORT, cursorLocation);
}

static void scroll() {
    int16_t blank = ' ' | (DEFAULT_SETTINGS << 8);
    if (screen_cursor_y >= SCREEN_SIZE_Y) {
        int i;
        for (i = 0 * SCREEN_SIZE_X; i < (SCREEN_SIZE_Y - 1) * SCREEN_SIZE_X; i++) {
            video_memory[i] = video_memory[i + SCREEN_SIZE_X];
        }
        int lastLine = SCREEN_SIZE_Y - 1;
        for (i = (lastLine) * SCREEN_SIZE_X; i < SCREEN_SIZE_Y * SCREEN_SIZE_X; i++) {
            video_memory[i] = blank;
        }
        screen_cursor_y = (lastLine);
    }
}

static void print(char c) {
    int16_t *location;
    location = video_memory + (screen_cursor_y * SCREEN_SIZE_X + screen_cursor_x);

    if (c != '\b') {
        *location = (c | (screen_settings << 8));
        if (++screen_cursor_x >= SCREEN_SIZE_X) {
            screen_cursor_x = 0;
            screen_cursor_y++;
        }
    } else {
        *location = (' ' | (screen_settings << 8));
    }
}

static void do_bell() {
    // TODO
}

static void do_backspace() {
    if (screen_cursor_x) {
        screen_cursor_x--;
    } else if (screen_cursor_y) {
        screen_cursor_x = SCREEN_SIZE_X - 1;
        screen_cursor_y--;
    }
    print('\b');
}

static void do_lineFeed() {
    screen_cursor_x = 0;
    screen_cursor_y++;
}

static void do_tab() {
    screen_cursor_x = (screen_cursor_x + 4) & ~(4 - 1);
}

static void do_return() {
    screen_cursor_x = 0;
}

static void screen_clear() {
    int16_t blank = ' ' | (DEFAULT_SETTINGS << 8);
    int i;
    for (i = 0; i < SCREEN_SIZE_X * SCREEN_SIZE_Y; i++) {
        video_memory[i] = blank;
    }
    screen_cursor_x = screen_cursor_y = 0;
    update_cursor();
}

static void do_scape_J() {
    if (screen_param[0] == 2) {
        screen_clear();
    }
}

/* Map from ANSI colors to the attributes used by the PC */
static uint8_t ansi_colors[8] = {0, 4, 2, 6, 1, 5, 3, 7};

static void do_scape_m() {
    int i;
    for (i = 0; i < screen_param_count; i++) {
        int dec = screen_param[i] / 10;
        int u = screen_param[i] % 10;
        if (dec == 0) {
            switch (u) {
                case 0:
                    screen_settings = DEFAULT_SETTINGS;
                    break;
                case 1:
                    screen_settings |= 0x08;
                    break;
                case 4:
                    screen_settings &= 0xBB;
                    break;
                case 5:
                    screen_settings |= 0x80;
            }
        } else if (dec == 3) { /* foreground */
            //print('3');
            screen_settings = (0xF0 & screen_settings) | (0x0F & ansi_colors[u]);
        } else if (dec == 4) { /* background */
            screen_settings = (0x0F & screen_settings) | (ansi_colors[u] << 4);
        }
    }
}

static void do_scape(char c) {
    switch (screen_state) {
        case 1:
            if (c == '[') {
                screen_state = 2;
                screen_param_count = 1;
                int i = 0;
                for (; i <= SCREEN_MAX_PARAM_COUNT; i++) {
                    screen_param[i] = 0;
                }
            } else {
                screen_state = 0;
            }
            break;
        case 2:
            if (c >= '0' && c <= '9') {
                screen_param[screen_param_count - 1] = 10 * screen_param[screen_param_count - 1] + (c - '0');
            } else if (c == ';') {
                screen_param_count++;
            } else {
                switch (c) {
                    case 'm':
                        do_scape_m();
                        break;
                    case 'J':
                        do_scape_J();
                        break;
                }
                screen_state = 0;
            }
            break;
    }
}

void screen_put(char c) {
    if (screen_state > 0) {
        do_scape(c);
        return;
    } else {
        switch (c) {
            case ESC:
                screen_state = 1;
                return;
            case '\0':
                return;
            case BELL:
                do_bell();
                return;
            case '\b':
                do_backspace();
                break;
            case '\n':
                do_lineFeed();
                break;
            case '\t':
                do_tab();
                break;
            case '\r':
                do_return();
                break;
            default:
                print(c);
                break;
        }
        scroll();
        update_cursor();
    }
}

void screen_write(char *string) {
    int i = 0;
    while (string[i]) {
        screen_put(string[i++]);
    }
}

static void timer_print(registers_t reg) {
    int i;
    for (i = 0; stdout.start != stdout.end; i++) {
        screen_put(stdout.array[stdout.start]);
        stdout.start = (stdout.start + 1) % stdout.size;
    }
}

void init_screen() {
    register_tick_subhandler(timer_print);
    stdout.start = stdout.end = 0;
    stdout.array = array_out;
    stdout.size = BUFFER_SIZE;
    add_in_out(1, &stdout);
    screen_write("\x1B[2J");
}
