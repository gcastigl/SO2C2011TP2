#ifndef TASK_H
#define TASK_H

#include <defs.h>
#include <paging/paging.h>

typedef struct task {
    int id;
    u32int esp, ebp;
    u32int eip;
    page_directory_t *page_directory;
    struct task *next;
} task_t;

void initialize_tasking();
void switch_task();
int fork();
void move_stack(void *new_stack_start, u32int size);
int getpid();

#endif