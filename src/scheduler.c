#include "../include/scheduler.h"

extern PROCESS idle;
extern PROCESS process[];
int firstTime = 1;

void saveEsp(int esp) {
    PROCESS *temp;
    int currPid = getCurrPid();
    if (!firstTime) {
        temp = getProcessByPid(currPid);
        temp->esp = esp;
    } else {
        firstTime = 0;
    }
    
    return;
}

int getNextProcess(int esp) {
    PROCESS* temp;
    if (!firstTime) {
        temp = getProcessByPid(getCurrPid());
        temp->esp = esp;
    } else {
        firstTime = 0;
    }
    temp = getNextTask();
    temp->lastCalled = 0;
    setCurrPid(temp->pid);
    
    return temp->esp;
}

PROCESS* getNextTask(void) {
    int i, winner, bestVal;
    bestVal = -1;
    if (noProcesses()) {
        return &idle;
    }
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (!process[i].free) {
            if (process[i].sleep >= 0) {
                if (process[i].blocked == 0) {
                    process[i].lastCalled++;
                    int temp;
                    temp = (MAX_PRIORITY - process[i].priority) * PRIORITY_RATIO + process[i].lastCalled;
                    if (temp > bestVal) {
                        bestVal = temp;
                        winner = i;
                    }
                }
            } else {
                process[i].sleep++;
            }
        }
    }
    printf("Winner: %d\n", winner);
    if (bestVal > -1) {
        return &process[winner];
    }
    return &idle;
}

void setupScheduler(void) {
    int i;
    void* stack = malloc(DEFAULT_STACK_SIZE);
    
    for (i = 0; i < MAX_PROCESSES; i++) {
        process[i].free = 1;
    }
    
    idle.pid = getNextPid();
    idle.foreground = 0;
    idle.priority = 4;
    strcpy(idle.name, "Idle");
    idle.sleep = 0;
    idle.blocked = 0;
    idle.tty = 0;
    idle.stackStart = (int)stack;
    idle.stackSize = DEFAULT_STACK_SIZE;
    idle.parent = 0;
    idle.lastCalled = 0;
    idle.free = 0;
    idle.esp = loadStackFrame(idle_process, 0, NULL, (int)(stack + DEFAULT_STACK_SIZE - 1), destroyProcess);
}