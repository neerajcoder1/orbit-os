.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax  # Get the pointer to the GDT, passed as a parameter
    lgdt (%eax)        # Load the new GDT pointer

    mov $0x10, %ax      # 0x10 is the offset in the GDT to our data segment
    mov %ax, %ds        # Load all data segment selectors
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08, $.flush   # 0x08 is the offset to our code segment: Far jump!
.flush:
    ret

.global idt_flush
idt_flush:
    mov 4(%esp), %eax  # Get the pointer to the IDT, passed as a parameter
    lidt (%eax)        # Load the IDT pointer
    ret

.global tss_flush
tss_flush:
    mov $0x2B, %ax      # Load the index of our TSS structure - The index is
                        # 0x28, as it is the 5th selector and each is 8 bytes
                        # long, but we set the bottom two bits (making 0x2B)
                        # so that it has an RPL of 3, not zero.
    ltr %ax             # Load task register
    ret

/* Macro for ISRs with NO error code (pushes dummy 0) */
.macro ISR_NOERRCODE num
  .global isr\num
  isr\num:
    cli
    push $0
    push $\num
    jmp isr_common_stub
.endm

/* Macro for ISRs WITH error code */
.macro ISR_ERRCODE num
  .global isr\num
  isr\num:
    cli
    push $\num
    jmp isr_common_stub
.endm

/* Macro for IRQs (Hardware Interrupts) */
.macro IRQ num, irq_num
  .global irq\irq_num
  irq\irq_num:
    cli
    push $0
    push $\num
    jmp irq_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_ERRCODE   21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

IRQ 32, 0
IRQ 33, 1
IRQ 34, 2
IRQ 35, 3
IRQ 36, 4
IRQ 37, 5
IRQ 38, 6
IRQ 39, 7
IRQ 40, 8
IRQ 41, 9
IRQ 42, 10
IRQ 43, 11
IRQ 44, 12
IRQ 45, 13
IRQ 46, 14
IRQ 47, 15

ISR_NOERRCODE 128

.extern isr_handler
isr_common_stub:
    pusha           # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov %ds, %ax    # Lower 16-bits of eax = ds
    push %eax       # save the data segment descriptor

    mov $0x10, %ax  # load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp       # Push pointer to the registers struct
    call isr_handler
    add $4, %esp    # Clean up the pushed pointer

    pop %eax        # reload the original data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa            # Pops edi,esi,ebp...
    add $8, %esp    # Cleans up the pushed error code and pushed ISR number
    sti
    iret            # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

.extern irq_handler
irq_common_stub:
    pusha           

    mov %ds, %ax
    push %eax       

    mov $0x10, %ax  
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp
    call irq_handler
    add $4, %esp

    pop %ebx        # reload the original data segment descriptor
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs

    popa            
    add $8, %esp    
    sti
    iret            
