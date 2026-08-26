#include "shell.h"
#include "../drivers/terminal.h"
#include "../drivers/keyboard.h"
#include "../kernel/string.h"
#include "../kernel/syscall.h"
#include "../fs/fs.h"
#include "../drivers/rtc.h"
#include "../drivers/pci.h"
#include "../drivers/e1000.h"
#include "../memory/pmm.h"

#define CMD_BUFFER_SIZE 256

static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_length = 0;

static void shell_prompt(void) {
    terminal_writestring("Orbit> ");
}

static void execute_command(char* cmd) {
    if (strlen(cmd) == 0) {
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        terminal_writestring("Available commands:\n");
        terminal_writestring("  help    - Show this help message\n");
        terminal_writestring("  clear   - Clear the terminal screen\n");
        terminal_writestring("  version - Display OS version\n");
        terminal_writestring("  memory  - Display memory statistics\n");
        terminal_writestring("  ls      - List files in rootfs\n");
        terminal_writestring("  cat     - Read a file (e.g. cat hello.txt)\n");
        terminal_writestring("  date    - Show current date and time\n");
        terminal_writestring("  lspci   - List PCI devices\n");
        terminal_writestring("  echo    - Print text to the screen (usage: echo <text>)\n");
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_initialize();
    } else if (strcmp(cmd, "version") == 0) {
        terminal_writestring("Orbit OS v0.1.0\n");
    } else if (strcmp(cmd, "memory") == 0) {
        char buf[32];
        terminal_writestring("Memory Statistics:\n");
        
        terminal_writestring("  Total Memory: ");
        terminal_writestring(itoa(pmm_get_total_memory() / 1024, buf, 10));
        terminal_writestring(" KB\n");
        
        terminal_writestring("  Used Memory:  ");
        terminal_writestring(itoa(pmm_get_used_memory() / 1024, buf, 10));
        terminal_writestring(" KB\n");
        
        terminal_writestring("  Free Memory:  ");
        terminal_writestring(itoa(pmm_get_free_memory() / 1024, buf, 10));
        terminal_writestring(" KB\n");
    } else if (strcmp(cmd, "ls") == 0) {
        if (!fs_root) {
            terminal_writestring("No filesystem mounted.\n");
            return;
        }
        int i = 0;
        struct dirent *node = 0;
        while ((node = fs_readdir(fs_root, i)) != 0) {
            fs_node_t *fsnode = fs_finddir(fs_root, node->name);
            if (fsnode) {
                if ((fsnode->flags & 0x07) == FS_DIRECTORY) {
                    terminal_writestring("(dir) ");
                } else {
                    terminal_writestring("(file) ");
                }
                terminal_writestring(node->name);
                terminal_writestring("\n");
            }
            i++;
        }
    } else if (strncmp(cmd, "cat ", 4) == 0) {
        char* filename = (char*)cmd + 4;
        if (!fs_root) {
            terminal_writestring("No filesystem mounted.\n");
            return;
        }
        fs_node_t* fsnode = fs_finddir(fs_root, filename);
        if (fsnode) {
            if ((fsnode->flags & 0x07) == FS_DIRECTORY) {
                terminal_writestring("Is a directory.\n");
            } else {
                char buf[256];
                uint32_t sz = fsnode->length;
                uint32_t offset = 0;
                while (sz > 0) {
                    uint32_t read_sz = sz > 255 ? 255 : sz;
                    uint32_t bytes = fs_read(fsnode, offset, read_sz, (uint8_t*)buf);
                    if (bytes == 0) break;
                    buf[bytes] = '\0';
                    terminal_writestring(buf);
                    offset += bytes;
                    sz -= bytes;
                }
                terminal_writestring("\n");
            }
        } else {
            terminal_writestring("File not found.\n");
        }
    } else if (strcmp(cmd, "date") == 0) {
        struct rtc_time time;
        rtc_get_time(&time);
        
        char buf[16];
        terminal_writestring(itoa(time.year, buf, 10)); terminal_writestring("-");
        
        if (time.month < 10) terminal_writestring("0");
        terminal_writestring(itoa(time.month, buf, 10)); terminal_writestring("-");
        
        if (time.day < 10) terminal_writestring("0");
        terminal_writestring(itoa(time.day, buf, 10)); terminal_writestring(" ");
        
        if (time.hour < 10) terminal_writestring("0");
        terminal_writestring(itoa(time.hour, buf, 10)); terminal_writestring(":");
        
        if (time.minute < 10) terminal_writestring("0");
        terminal_writestring(itoa(time.minute, buf, 10)); terminal_writestring(":");
        
        if (time.second < 10) terminal_writestring("0");
        terminal_writestring(itoa(time.second, buf, 10)); terminal_writestring("\n");
    } else if (strcmp(cmd, "lspci") == 0) {
        pci_enumerate();
    } else if (strcmp(cmd, "netinfo") == 0) {
        if (e1000_is_found()) {
            uint8_t mac[6];
            e1000_get_mac(mac);
            char buf[16];
            terminal_writestring("Intel E1000 (8254x) Gigabit Ethernet\n");
            terminal_writestring("MAC Address: ");
            char* hex = "0123456789ABCDEF";
            for (int i = 0; i < 6; i++) {
                terminal_putchar(hex[mac[i] >> 4]);
                terminal_putchar(hex[mac[i] & 0x0F]);
                if (i < 5) terminal_putchar(':');
            }
            terminal_writestring("\n");
        } else {
            terminal_writestring("No compatible network adapter found.\n");
        }
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        terminal_writestring(cmd + 5);
        terminal_writestring("\n");
    } else {
        terminal_writestring("Unknown command: ");
        terminal_writestring(cmd);
        terminal_writestring("\n");
    }
}

void shell_initialize(void) {
    cmd_length = 0;
    memset(cmd_buffer, 0, CMD_BUFFER_SIZE);
}

void shell_run(void) {
    terminal_writestring("Welcome to Orbit OS Shell!\nType 'help' for a list of commands.\n");
    shell_prompt();

    while (1) {
        char c = keyboard_getchar();

        if (c == '\n') {
            terminal_putchar('\n');
            cmd_buffer[cmd_length] = '\0';
            execute_command(cmd_buffer);
            cmd_length = 0;
            shell_prompt();
        } else if (c == '\b') {
            if (cmd_length > 0) {
                cmd_length--;
                terminal_putchar('\b');
            }
        } else if (c >= 32 && c <= 126) { /* Printable characters */
            if (cmd_length < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_length++] = c;
                terminal_putchar(c);
            }
        }
    }
}
