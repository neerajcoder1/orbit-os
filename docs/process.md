# Process Management (Phase 8)

Orbit OS implements preemptive multitasking at the kernel level using the Programmable Interval Timer (PIT).

## Process Control Block (PCB)
The PCB (`process_t`) stores the metadata for a task, including its ID, `esp` (stack pointer), and `cr3` (page directory). Since the OS does not yet have a heap, PCBs are allocated as full 4 KB blocks from the Physical Memory Manager.

## Context Switching
Context switching is executed entirely in Assembly (`process/switch.s`). 
When a context switch occurs, the function:
1. Pushes the callee-saved registers (`ebp`, `ebx`, `esi`, `edi`) onto the current stack.
2. Saves the current `esp` into the active PCB.
3. Retrieves the `cr3` for the next task but does not change it yet.
4. Updates the CPU's `esp` with the next task's stack pointer.
5. Flushes the Page Directory by updating `cr3`.
6. Pops the callee-saved registers from the new stack and returns, seamlessly resuming the next task's execution.

## Thread Creation
To create a thread, `process_create_thread` allocates a fresh stack and manually constructs a mock stack frame. It pushes the address of `thread_exit`, followed by the thread's actual `entry_point`, and initializes dummy registers. When the scheduler switches to this mock stack for the first time, the `ret` instruction pulls `entry_point` off the stack and begins execution.

## Preemptive Scheduler
The scheduler is driven by the PIT on IRQ0 (Interrupt 32). Every 50 ticks (which equates to 0.5 seconds at the PIT's 100 Hz frequency), the PIT callback invokes `process_schedule()`. The scheduler rotates the circular run-queue and performs a context switch. Because x86 interrupt handling inherently saves the `EFLAGS` register, preempted tasks will correctly restore their interrupt-enabled (`IF`) state when they are resumed.
