#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "kernel.h"
#include "command.h"

#define DEFAULT_STACK_SIZE 0x200

int getNextProcess(int esp);
PROCESS* getNextTask(void);
#endif