# System Calls (Phase 9)

System calls (syscalls) provide a controlled bridge between user-space applications (Ring 3) and the kernel (Ring 0). They allow programs to request privileged operations like file I/O, process management, and memory allocation.

## Interrupt 0x80 (`int 0x80`)
Orbit OS uses the classic POSIX-style `int 0x80` software interrupt to trap into the kernel. 
- **IDT Configuration:** In `kernel/isr.c`, Interrupt 128 (0x80) is mapped to `isr128` and given a Descriptor Privilege Level (DPL) of 3 (`0xEE`). This specifically allows unprivileged user-mode applications to trigger this interrupt without causing a General Protection Fault.
- **Syscall Dispatcher:** The `syscall_handler` in `kernel/syscall.c` catches the `int 0x80` interrupt and reads the `EAX` register to determine which system call was requested. 

## Syscall Implementation
Currently, Orbit OS implements the `SYS_WRITE` (4) syscall as a proof-of-concept.
- **EAX = 4** (syscall number)
- **EBX = 1** (file descriptor for STDOUT)
- **ECX = pointer** (address of the string to print)
- **EDX = length** (number of characters)

When the syscall is executed, the kernel safely prints the string via the VGA Terminal driver and the COM1 Serial driver, then returns execution back to the caller.
