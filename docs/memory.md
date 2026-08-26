# Memory Management (Phase 7)

Orbit OS implements a two-tier memory management system: a Physical Memory Manager (PMM) and a Virtual Memory Manager (VMM).

## Physical Memory Manager (PMM)
The PMM is responsible for tracking which physical RAM blocks are free and which are in use.
- **Multiboot Memory Map:** GRUB provides a memory map detailing available RAM. The PMM parses this map to identify usable regions.
- **Bitmap Allocator:** The PMM divides available memory into 4096-byte (4 KB) blocks. It tracks their state using a bitmap placed immediately after the kernel image in memory (`kernel_end`).
- **Initialization:** By default, all memory is marked as used. The PMM scans the multiboot map and clears the bits for available, non-kernel regions.

## Virtual Memory Manager (VMM)
The VMM implements x86 hardware paging, allowing the OS to map virtual addresses to physical addresses.
- **Page Directory & Tables:** The VMM creates a Page Directory and allocates Page Tables using the PMM. 
- **Identity Mapping:** During initialization, the VMM identity-maps the first 8 MB of memory. This ensures that the kernel code, the VGA text buffer (`0xB8000`), and the multiboot data structures remain accessible at their physical addresses once paging is enabled.
- **CR3 & CR0:** Paging is enabled by writing the physical address of the Page Directory to the `CR3` register and setting the highest bit of the `CR0` register.

## Shell Integration
You can view the current state of the physical memory allocator by typing `memory` in the Orbit Shell. This uses a custom integer-to-ascii (`itoa`) routine to format the memory statistics in kilobytes.
