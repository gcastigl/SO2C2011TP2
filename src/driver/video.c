#include "../../include/driver/video.h"

void video_init() {
	video.address = (char*) VIDEO_ADDRESS;
	terminal_clearAll(video.address, -1);
	video.offset = 0;
	video_setCursor(0);
}

void video_write(char *string, size_t count, char format) {
	memcpy(video.address + video.offset, string, count);
	video.offset += count * 2;
	video_setCursor(video.offset / 4);
}

void video_setCursor(int offset) {
	if (0 <= offset && offset < ROWS * COLUMNS) {
		// cursor LOW port to vga INDEX register
		outb(0x3D4, 0x0F);
		outb(0x3D5, (ushort)(offset & 0xFF));
		// cursor HIGH port to vga INDEX register
		outb(0x3D4, 0x0E);
		outb(0x3D5, (ushort)((offset >> 8) & 0xFF));
	}
}

void video_clearScreen(char format) {
	terminal_clearAll(video.address, format);
	video.offset = 0;
	video_setCursor(0);
}

void video_clearRow(int row, char format) {
	terminal_clearRow(video.address, row, format);
}

void video_setOffset(int offset) {
	video.offset = offset;
}

char video_getFormattedColor(char fg, char bg) {
	return (bg << 4) | (fg & 0x0F);
}

char video_getBGcolor(char format) {
	return format >> 4;
}

char video_getFGcolor(char format) {
	return format & 0x0F;
}
