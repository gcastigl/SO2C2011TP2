#ifndef MAIN_H
#define MAIN_H

#include <defs.h>
#include <fs/fs.h>
#include <shell.h>
#include <multiboot.h>
#include <interrupts/interrupts.h>
#include <driver/video.h>
#include <driver/ata_disk.h>
#include <asm/libasm.h>
#include <io.h>
#include <lib/stdio.h>
#include <paging/paging.h>
#include <process/task.h>
#include <process/process.h>
#include <command.h>
#include <access/user.h>
#include <driver/port.h>

/* getNextPID
*
* Devuelve el próximo PID libre
**/
int getNextPID();

#endif

