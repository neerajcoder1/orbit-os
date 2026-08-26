# Orbit OS

Orbit OS is an educational operating system built from scratch. The primary goal is to explore operating-system internals through a modular, well-structured, and incrementally designed C/x86 codebase.

## Features (Current Phase: Developer Dashboard / Completed)
- Multiboot 1 compliant bootloader (GRUB)
- Custom x86 linker script and memory layout
- 32-bit Protected Mode kernel with GDT and IDT
- Interrupts (ISR/IRQ) and Programmable Interrupt Controller (PIC)
- Virtual Memory (VMM) and Physical Memory (PMM) management
- Basic Preemptive Multitasking and User-Space Transitions
- Virtual File System (VFS) with an initrd custom flat file format
- PCI Bus Enumeration and E1000 Gigabit Ethernet Detection
- Orbit Shell with interactive commands (`ls`, `cat`, `date`, `lspci`, `netinfo`)
- Full-Stack Developer Dashboard (React + FastAPI) for automated testing

## Architecture
- **Bootloader**: GRUB via standard ISO 9660 format
- **Kernel**: 32-bit x86 ELF
- **Language**: C (gnu99), Assembly (x86)
- **Build System**: GNU Make & GCC

## Directory Structure
```
orbit-os/
├── boot/           # Bootloader configurations and assembly entry
├── kernel/         # Core kernel C files
├── drivers/        # Hardware device drivers (planned)
├── memory/         # Memory management (planned)
├── filesystem/     # Filesystem implementation (planned)
├── process/        # Process and threading (planned)
├── syscall/        # System call interface (planned)
├── shell/          # Command-line shell (planned)
├── user/           # User-space programs (planned)
├── libc/           # Standalone standard library (planned)
├── tools/          # Build and development tools (planned)
├── tests/          # Testing framework
├── docs/           # Documentation
├── Makefile        # Main build script
└── linker.ld       # Memory layout configuration
```

## Requirements
To build and run Orbit OS, you need:
- `gcc` (with 32-bit multilib support) or an `i686-elf-gcc` cross-compiler
- `as` (GNU Assembler)
- `grub-mkrescue` and `xorriso` (for ISO generation)
- `qemu-system-i386` (for running the OS)
- `make`

## Build Instructions
Run the following command to compile the kernel and generate the bootable ISO:
```sh
make all iso
```
*Note: A `Dockerfile` and `build.ps1` script are included for convenient building on Windows using Docker Desktop.*

## QEMU Instructions
To run the generated ISO in an emulator:
```sh
make run
```
To run with GDB debugging support:
```sh
make debug
```

## Roadmap
- [x] Phase 1: Project Foundation
- [x] Phase 2: Bootable Kernel
- [x] Phase 3: Terminal Abstraction
- [x] Phase 4: CPU and Interrupts (GDT/IDT)
- [x] Phase 5: Keyboard Driver
- [x] Phase 6: Orbit Shell
- [x] Phase 7: Memory Management
- [x] Phase 8: Process Management
- [x] Phase 9: System Calls
- [x] Phase 10: User Space
- [x] Phase 11: Filesystem
- [x] Phase 12: Device Drivers
- [x] Phase 13: Networking
- [x] Phase 14: Developer Dashboard
