AS = as --32
CC = gcc
CFLAGS = -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
LDFLAGS = -m32 -ffreestanding -O2 -nostdlib -no-pie -Wl,--build-id=none
QEMU = qemu-system-i386

OBJS = boot/boot.o kernel/kernel.o kernel/string.o kernel/syscall.o drivers/terminal.o kernel/gdt.o kernel/idt.o kernel/isr.o kernel/interrupts.o drivers/pic.o drivers/pit.o drivers/keyboard.o drivers/rtc.o drivers/pci.o drivers/e1000.o shell/shell.o memory/pmm.o memory/vmm.o process/process.o process/switch.o process/user.o fs/fs.o fs/initrd.o

.PHONY: all clean iso build run debug

all: build

build: orbitos.bin

%.o: %.s
	$(AS) -o $@ $<

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

orbitos.bin: $(OBJS)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJS)

isodir/boot/grub/grub.cfg: boot/grub/grub.cfg
	mkdir -p isodir/boot/grub
	cp boot/grub/grub.cfg isodir/boot/grub/grub.cfg

iso: orbitos.iso

orbitos.iso: orbitos.bin isodir/boot/grub/grub.cfg
	mkdir -p isodir/boot
	cp orbitos.bin isodir/boot/orbitos.bin
	python3 mkfs.py initrd.bin rootfs
	cp initrd.bin isodir/boot/initrd.bin
	grub-mkrescue -o orbitos.iso isodir

run: orbitos.iso
	$(QEMU) -cdrom orbitos.iso

debug: orbitos.iso
	$(QEMU) -S -s -cdrom orbitos.iso

clean:
	rm -f $(OBJS) orbitos.bin orbitos.iso initrd.bin
	rm -rf isodir
