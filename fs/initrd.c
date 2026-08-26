#include "initrd.h"
#include "../kernel/string.h"
#include "../memory/pmm.h"

struct initrd_header {
    uint32_t magic;
    uint32_t num_files;
} __attribute__((packed));

struct initrd_file_header {
    char name[64];
    uint32_t offset;
    uint32_t length;
} __attribute__((packed));

#define MAX_FILES 64

static fs_node_t initrd_nodes[MAX_FILES];
static uint32_t num_files = 0;
static fs_node_t root_node;
static uint32_t initrd_location;

static uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (offset >= node->length) return 0;
    if (offset + size > node->length) {
        size = node->length - offset;
    }
    
    uint8_t* data = (uint8_t*)(node->impl + offset);
    memcpy(buffer, data, size);
    return size;
}

static dirent_t dir_cache;

static dirent_t* initrd_readdir(fs_node_t* node, uint32_t index) {
    if (node != &root_node) return 0;
    if (index >= num_files) return 0;
    
    strcpy(dir_cache.name, initrd_nodes[index].name);
    dir_cache.ino = initrd_nodes[index].inode;
    return &dir_cache;
}

static fs_node_t* initrd_finddir(fs_node_t* node, char* name) {
    if (node != &root_node) return 0;
    for (uint32_t i = 0; i < num_files; i++) {
        if (strcmp(initrd_nodes[i].name, name) == 0) {
            return &initrd_nodes[i];
        }
    }
    return 0;
}

void initrd_initialize(uint32_t location) {
    initrd_location = location;
    
    strcpy(root_node.name, "root");
    root_node.flags = FS_DIRECTORY;
    root_node.readdir = initrd_readdir;
    root_node.finddir = initrd_finddir;
    fs_root = &root_node;
    
    struct initrd_header* header = (struct initrd_header*)location;
    if (header->magic != 0x5442524F) { // 'ORBT'
        return;
    }
    
    num_files = header->num_files;
    if (num_files > MAX_FILES) num_files = MAX_FILES;
    
    struct initrd_file_header* file_headers = (struct initrd_file_header*)(location + 8);
    
    for (uint32_t i = 0; i < num_files; i++) {
        fs_node_t* node = &initrd_nodes[i];
        strcpy(node->name, file_headers[i].name);
        node->flags = FS_FILE;
        node->length = file_headers[i].length;
        node->impl = location + file_headers[i].offset;
        node->inode = i;
        node->read = initrd_read;
    }
}
