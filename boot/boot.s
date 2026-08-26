/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

/* 
Declare a header as in the Multiboot Standard.
*/
.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 /* 16 KiB */
stack_top:

/*
The linker script specifies _start as the entry point to the kernel.
*/
.section .text
.global _start
.type _start, @function
_start:
	/*
	To set up a stack, we set the esp register to point to the top of our
	stack (as it grows downwards on x86 systems).
	*/
	mov $stack_top, %esp

	/*
	Transfer control to the main kernel.
	*/
	push %ebx       /* Multiboot info structure pointer */
	push %eax       /* Multiboot magic number */
	call kernel_main
	add $8, %esp    /* Clean up stack */

	/*
	If the system has nothing more to do, put the computer into an
	infinite loop.
	*/
	cli
1:	hlt
	jmp 1b

/*
Set the size of the _start symbol.
*/
.size _start, . - _start
