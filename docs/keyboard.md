# Keyboard Driver (Phase 5)

## PS/2 Controller
The keyboard driver (`drivers/keyboard.c`) interacts with the legacy PS/2 controller via I/O port `0x60`. When a key is pressed or released on the keyboard, the controller fires an interrupt on IRQ1 (Interrupt 33).

## Handling Scancodes
The driver reads a single byte from `0x60` which contains the PS/2 Scan Code (Set 1). 
- If the high bit (`0x80`) is set, the key was released (break code).
- If the high bit is clear, the key was pressed (make code).
- The driver translates the scancode into a standard ASCII character using a lookup table. 
- It tracks the state of the Shift keys (`0x2A` and `0x36`) to select between upper and lower case translation tables.

## Ring Buffer
Translated characters are placed into a 256-byte ring buffer (`keyboard_buffer`). 
The `buffer_head` tracks where to write new characters (producer), while `buffer_tail` tracks where to read characters (consumer).

## Reading Input
The driver exposes a blocking API `keyboard_getchar()`. If the buffer is empty, it halts the CPU (`hlt`) to save power until an interrupt fires. Once a key is pressed, it extracts the oldest character from the tail of the buffer and returns it to the caller.
