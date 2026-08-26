#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "io.h"
#include "../drivers/terminal.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "../drivers/pit.h"
#include "../drivers/keyboard.h"
#include "../shell/shell.h"

#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../process/process.h"
#include "syscall.h"
#include "../fs/fs.h"
#include "../fs/initrd.h"
#include "../drivers/rtc.h"
#include "../drivers/pci.h"
#include "../drivers/e1000.h"

void serial_write_char(char c) {
    outb(0x3F8, c);
}

void serial_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

void user_mode_function(void) {
    while (1) {
        for(volatile int i = 0; i < 5000000; i++);
    }
}

void background_task(void) {
    enter_user_mode(user_mode_function);
}

void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
	terminal_initialize();

	const char* msg = 
		"================================\n"
		"          ORBIT OS\n"
		"================================\n"
		"Kernel loaded successfully.\n"
		"Version 0.1.0\n"
		"Architecture x86\n"
		"================================\n";

	terminal_writestring(msg);
	
	/* Initialize COM1 (basic, no interrupts) */
	outb(0x3F8 + 1, 0x00);
	outb(0x3F8 + 3, 0x80);
	outb(0x3F8 + 0, 0x03);
	outb(0x3F8 + 1, 0x00);
	outb(0x3F8 + 3, 0x03);
	outb(0x3F8 + 2, 0xC7);
	outb(0x3F8 + 4, 0x0B);

	serial_write_string(msg);

    if (magic != 0x2BADB002) {
        terminal_writestring("ERROR: Invalid Multiboot Magic!\n");
        return;
    }

    terminal_writestring("Initializing CPU...\n");
    gdt_initialize();
    idt_initialize();
    isr_initialize();
    pit_initialize(100);
    
    keyboard_initialize();
    terminal_writestring("Hardware initialized.\n");

    pmm_initialize(mbi);
    vmm_initialize();
    terminal_writestring("Memory initialized.\n");
    
    rtc_initialize();
    pci_initialize();
    e1000_initialize();

    if (mbi->flags & (1 << 3)) { // If modules are present
        if (mbi->mods_count > 0) {
            multiboot_module_t* mod = (multiboot_module_t*)mbi->mods_addr;
            
            initrd_initialize(mod->mod_start);
            terminal_writestring("Initrd filesystem mounted. Files:\n");
            
            for (int i = 0; i < 10; i++) {
                struct dirent* d = fs_readdir(fs_root, i);
                if (d) {
                    terminal_writestring(" - ");
                    terminal_writestring(d->name);
                    terminal_writestring("\n");
                }
            }

            // Test VFS!
            fs_node_t* hello_file = fs_finddir(fs_root, "hello.txt");
            if (hello_file) {
                char buf[256];
                uint32_t bytes = fs_read(hello_file, 0, hello_file->length, (uint8_t*)buf);
                buf[bytes] = '\0';
                terminal_writestring("Read from VFS (hello.txt): ");
                terminal_writestring(buf);
                terminal_writestring("\n");
            } else {
                terminal_writestring("ERROR: hello.txt not found in initrd.\n");
            }
        }
    } else {
        terminal_writestring("WARNING: No initrd module loaded by GRUB.\n");
    }

    process_initialize();
    process_create_thread(background_task);
    terminal_writestring("Process manager initialized. Spawned background thread.\n");

    syscall_initialize();
    terminal_writestring("Syscalls initialized.\n");

    /* Enable interrupts */
    __asm__ volatile("sti");
    terminal_writestring("Interrupts enabled.\n\n");

    /* Initialize and hand over control to the shell */
    shell_initialize();
    shell_run();
}
