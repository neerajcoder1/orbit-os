.global enter_user_mode

/*
 * void enter_user_mode(void (*user_func)(void));
 *
 * Drops execution to Ring 3 (User Space).
 * Arguments:
 *   [esp+4] = address of the user mode function to execute
 */
enter_user_mode:
    cli

    # Get the function pointer
    mov 4(%esp), %ebx

    # Set up data segments for user mode (0x20 | 3 = 0x23)
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # We need to construct an iret frame to transition to Ring 3
    # Stack layout for iret to Ring 3:
    # SS, ESP, EFLAGS, CS, EIP

    mov %esp, %eax      # Save current ESP for the user stack
                        # (Normally we'd allocate a separate user stack, 
                        # but for this simple test, we reuse the current stack)

    push $0x23          # Push SS (User Data Segment)
    push %eax           # Push ESP
    pushf               # Push EFLAGS
    
    # Enable interrupts in EFLAGS (OR with 0x200)
    pop %eax
    or $0x200, %eax
    push %eax           # Push modified EFLAGS

    push $0x1B          # Push CS (User Code Segment, 0x18 | 3 = 0x1B)
    push %ebx           # Push EIP (the user function)

    iret                # Jump to user mode!
