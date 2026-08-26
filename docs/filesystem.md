# Filesystem

Orbit OS includes a simple memory-based Virtual File System (VFS).

## Virtual File System (VFS)

The VFS provides an abstraction layer that allows the OS to support multiple filesystems. It is defined in `fs/fs.h`. Every file or directory is represented by a `fs_node_t` struct, which includes function pointers for standard operations:
- `read`: Read data from a file
- `write`: Write data to a file (not supported in read-only filesystems)
- `readdir`: Read a directory entry by index
- `finddir`: Find a directory entry by name

## initrd (Initial Ramdisk)

The initial filesystem is loaded by GRUB as a multiboot module (`initrd.bin`).
We use a custom, simple, flat file format built by `mkfs.py`.
The memory allocation manager (PMM) parses the multiboot modules list and protects the `initrd` memory location from being allocated or overwritten.

### Initrd Format
1. **Header (8 bytes):**
   - 4 bytes magic number (`ORBT`)
   - 4 bytes number of files (32-bit little endian)
2. **File Headers (72 bytes each):**
   - 64 bytes for the file name (null-padded)
   - 4 bytes offset to the file data (from start of archive)
   - 4 bytes for file length
3. **Data Blocks:** Flat file contents at the offsets specified in headers.

## Shell Integration

The shell includes basic filesystem commands:
- `ls`: Uses `fs_readdir` to list files in the root directory.
- `cat <file>`: Uses `fs_finddir` to locate a file, and `fs_read` to read its contents to the screen.
