#include "keyboard.h"
#include "../kernel/io.h"
#include "../kernel/isr.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_BUFFER_SIZE 256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_head = 0;
static volatile int buffer_tail = 0;

static bool shift_pressed = false;

/* Simple US QWERTY translation table (Scan Code Set 1) */
static const char scancode_ascii_lower[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Ctrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right shift */
    '*',
    0, /* Alt */
    ' ', /* Space */
    0, /* Caps lock */
};

static const char scancode_ascii_upper[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, /* Ctrl */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, /* Right shift */
    '*',
    0, /* Alt */
    ' ', /* Space */
    0, /* Caps lock */
};

static void keyboard_callback(registers_t* regs) {
    (void)regs;

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    /* Handle shift keys */
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = false;
        return;
    }

    /* Top bit set means key release */
    if (scancode & 0x80) {
        return;
    }

    char c = 0;
    if (scancode < 128) {
        if (shift_pressed) {
            c = scancode_ascii_upper[scancode];
        } else {
            c = scancode_ascii_lower[scancode];
        }
    }

    /* Push to ring buffer */
    if (c != 0) {
        int next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        if (next_head != buffer_tail) {
            keyboard_buffer[buffer_head] = c;
            buffer_head = next_head;
        }
    }
}

void keyboard_initialize(void) {
    /* Register IRQ1 (Interrupt 33) */
    register_interrupt_handler(33, keyboard_callback);
}

bool keyboard_haskey(void) {
    return buffer_head != buffer_tail;
}

char keyboard_getchar(void) {
    /* Block until a key is available */
    while (!keyboard_haskey()) {
        __asm__ volatile("hlt");
    }

    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}
