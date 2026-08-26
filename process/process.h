#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

typedef struct process {
    uint32_t id;
    uint32_t esp;
    uint32_t kernel_stack;
    uint32_t page_directory;
    struct process* next;
} process_t;

void process_initialize(void);
process_t* process_create_thread(void (*entry_point)(void));
void process_schedule(void);
void enter_user_mode(void (*user_func)(void));

#endif
