# Booting Orbit OS

## The Boot Sequence
1. **BIOS / UEFI**: The system firmware initializes hardware and loads the bootloader.
2. **GRUB (Bootloader)**: GRUB is configured via `grub.cfg` to load the `orbitos.bin` kernel as a Multiboot payload.
3. **Multiboot Entry (`boot.s`)**: The assembly stub provides the Multiboot header (Magic `0x1BADB002`) in the `.multiboot` section. It sets up the kernel stack (`stack_top`) and calls `kernel_main`.
4. **Kernel (`kernel.c`)**: Control is handed to C code. The kernel initializes the VGA text buffer, clears the screen, and outputs the welcome message.

## Memory Layout
The linker script (`linker.ld`) places the kernel at `1M` (1 MiB), which is the standard load address for x86 kernels to avoid conflicts with BIOS and memory-mapped IO. The `.multiboot` section is guaranteed to be near the beginning of the binary to satisfy the Multiboot specification.
