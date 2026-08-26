.global switch_task

/*
 * switch_task(uint32_t *current_esp, uint32_t next_esp, uint32_t next_cr3);
 *
 * This function performs a cooperative context switch between two tasks.
 * It saves the callee-saved registers (ebx, esi, edi, ebp) onto the current stack,
 * saves the current ESP into *current_esp,
 * then loads the new ESP from next_esp,
 * and pops the callee-saved registers from the new stack before returning.
 */
switch_task:
    # Save the current state
    push %ebp
    push %ebx
    push %esi
    push %edi

    # Get the address where we should save the current ESP
    mov 20(%esp), %eax
    mov %esp, (%eax)

    # Get the next CR3 (Page Directory) BEFORE changing ESP
    mov 28(%esp), %eax

    # Get the next ESP and change stack
    mov 24(%esp), %esp

    # Now safe to change CR3
    mov %eax, %cr3

    # Restore the next state
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp

    ret
