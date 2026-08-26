# User Space (Phase 10)

User Space (Ring 3) is the lowest privilege level in the x86 architecture. Applications running in User Space cannot directly access hardware, modify page tables, disable interrupts, or halt the CPU. They must use System Calls to request the Kernel to perform these actions on their behalf.

## Global Descriptor Table (GDT) and Task State Segment (TSS)
To support Ring 3, the GDT (`kernel/gdt.c`) was expanded from 3 to 6 entries:
1. `0x08`: Kernel Code (Ring 0)
2. `0x10`: Kernel Data (Ring 0)
3. `0x18`: User Code (Ring 3)
4. `0x20`: User Data (Ring 3)
5. `0x28`: Task State Segment (TSS)

The **TSS** is a special hardware structure that tells the CPU where to find the kernel stack (`esp0`) when a hardware interrupt or system call occurs while executing in User Space. Each time the scheduler performs a context switch, it updates `tss.esp0` to point to the active thread's kernel stack.

## Dropping to Ring 3
To transition from the Kernel (Ring 0) to User Space (Ring 3), the OS uses the `iret` (Interrupt Return) instruction. `enter_user_mode` (`process/user.s`) constructs a mock interrupt frame on the stack containing:
- `SS` (User Data Segment: `0x23`)
- `ESP` (User Stack Pointer)
- `EFLAGS` (With Interrupts Enabled flag `0x200`)
- `CS` (User Code Segment: `0x1B`)
- `EIP` (Address of the user function)

When `iret` executes, the CPU pops these values and seamlessly transitions into Ring 3 execution.

## Memory Protection
The Virtual Memory Manager (`memory/vmm.c`) was updated to tag the identity-mapped pages with the `I86_PDE_USER` flag. If this flag is omitted, any code executing in Ring 3 will immediately trigger a Page Fault when trying to fetch instructions or read data.
