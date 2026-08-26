#include "process.h"
#include "../memory/pmm.h"
#include "../kernel/string.h"
#include "../drivers/terminal.h"
#include "../kernel/gdt.h" // For tss_set_stack

extern void switch_task(uint32_t* current_esp, uint32_t next_esp, uint32_t next_cr3);

static process_t* current_process = 0;
static process_t* process_queue = 0;
static uint32_t next_pid = 1;

/* We need to define a simple kmalloc to allocate PCBs.
   Since we don't have a heap yet, we will just allocate full 4KB pages for PCBs to keep it simple!
   (In a real OS, a slab allocator or heap would be used). */
static void* simple_kmalloc(void) {
    return pmm_alloc_block();
}

void process_initialize(void) {
    /* Create the main kernel thread (the currently running code) */
    current_process = (process_t*)simple_kmalloc();
    current_process->id = next_pid++;
    current_process->next = current_process; // Circular list
    
    // We don't have a known kernel stack top for the boot thread initially, 
    // but we can just use the current ESP + some arbitrary safe value,
    // or rely on the bootloader stack. For now, we'll just read esp.
    uint32_t esp;
    __asm__ volatile("mov %%esp, %0" : "=r"(esp));
    current_process->kernel_stack = esp + 4096; // Approximation

    // We don't need to initialize ESP/CR3 right now because they will be saved
    // into this struct the first time switch_task is called.
    // However, we should set the page directory to the current CR3.
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    current_process->page_directory = cr3;

    process_queue = current_process;
}

static void thread_exit(void) {
    terminal_writestring("Thread exited!\n");
    while (1) {
        __asm__ volatile("hlt");
    }
}

process_t* process_create_thread(void (*entry_point)(void)) {
    process_t* proc = (process_t*)simple_kmalloc();
    proc->id = next_pid++;

    uint32_t stack_base = (uint32_t)simple_kmalloc();
    uint32_t stack_top = stack_base + 4096;
    proc->kernel_stack = stack_top;

    uint32_t* stack = (uint32_t*)stack_top;

    /* Push the exit function so if the thread returns, it safely halts */
    *(--stack) = (uint32_t)thread_exit;
    /* Push the entry point for 'ret' in switch_task */
    *(--stack) = (uint32_t)entry_point;

    /* Push initial values for ebp, ebx, esi, edi (the ones switch_task pops) */
    *(--stack) = 0; // edi
    *(--stack) = 0; // esi
    *(--stack) = 0; // ebx
    *(--stack) = 0; // ebp

    proc->esp = (uint32_t)stack;

    /* Use the same page directory as the kernel */
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    proc->page_directory = cr3;

    /* Add to run queue (circular list) */
    process_t* tail = process_queue;
    while (tail->next != process_queue) {
        tail = tail->next;
    }
    tail->next = proc;
    proc->next = process_queue;

    return proc;
}

void process_schedule(void) {
    if (!current_process || current_process->next == current_process) {
        return; // Nothing to schedule
    }

    process_t* prev = current_process;
    current_process = current_process->next;

    // Update TSS so user-mode interrupts use this task's kernel stack
    tss_set_stack(0x10, current_process->kernel_stack);

    switch_task(&prev->esp, current_process->esp, current_process->page_directory);
}
