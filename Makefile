OBJ_DIR=obj/
BIN_DIR=bin/
NAME=kernel.bin

SOURCES= src/asm/libasm.asm src/asm/boot.asm src/libc.c \
src/driver/keyboard.c \
src/lib/stdlib.c src/lib/math.c \
src/shell.c src/command.c src/interrupts.c src/lib/string.c src/lib/printf.c src/lib/scanf.c src/io.c src/driver/videoUtils.c src/driver/video.c src/user.c src/main.c

OBJS=$(patsubst src%.asm, bin%.o,  \
$(patsubst src%.c, bin%.o, $(SOURCES)))

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -m32 -g
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-faout
CC=gcc

C_INCLUDE_PATH=include
NAME=kernel.bin

define cc-command
$(CC) $(CFLAGS) -I$(C_INCLUDE_PATH) $< -o $@
endef

all: $(OBJS) link

clean:
	rm bin/*.o bin/$(NAME)

link:
	ld $(LDFLAGS) -o $(BIN_DIR)/$(NAME) $(OBJS)

bin/%.o: src/%.c
	mkdir -p $(dir $@)
	$(cc-command)

bin/%.o: src/%.asm
	mkdir -p $(dir $@)
	nasm $(ASFLAGS) $< -o $@