#include "syscall.h"
#include "isr.h"
#include "../drivers/terminal.h"

// Standard POSIX-like syscall numbers (simplified)
#define SYS_WRITE 4

// Syscall handler prototype
typedef void (*syscall_handler_t)(registers_t* regs);

static void sys_write(registers_t* regs) {
    // EAX = syscall number (4)
    // EBX = file descriptor (1 = stdout)
    // ECX = pointer to string
    // EDX = length of string
    
    int fd = regs->ebx;
    const char* str = (const char*)regs->ecx;
    // size_t len = regs->edx; // Not strictly enforced right now
    
    if (fd == 1 || fd == 2) { // stdout or stderr
        terminal_writestring(str);
        regs->eax = regs->edx; // Return bytes written
    } else {
        regs->eax = -1; // Error
    }
}

static void syscall_handler(registers_t* regs) {
    // Determine the syscall number from EAX
    switch (regs->eax) {
        case SYS_WRITE:
            sys_write(regs);
            break;
        default:
            terminal_writestring("Unknown syscall: ");
            // Print error or something
            break;
    }
}

void syscall_initialize(void) {
    // Register ISR 128 (0x80) with the interrupt dispatcher
    register_interrupt_handler(128, syscall_handler);
}
