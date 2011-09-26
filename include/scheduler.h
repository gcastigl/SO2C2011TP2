#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "kernel.h"
#include "command.h"

PROCESS* getNextTask(void);
int loadEsp(PROCESS* process);
void* getTempEsp(void);
void saveEsp(int esp);
#endif