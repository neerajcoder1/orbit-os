# Orbit Shell (Phase 6)

## Overview
The Orbit Shell is the primary user interface for interacting with the operating system before a full graphical user interface or user-space standard library is available. It runs entirely in kernel space (Ring 0) and utilizes the keyboard driver and VGA terminal driver.

## Features
- **Prompt:** Displays an interactive `Orbit> ` prompt.
- **Input Buffering:** Reads characters using `keyboard_getchar()`, echoing them to the terminal. It buffers up to 256 characters before executing.
- **Backspace Support:** Natively supports backspacing over typos, seamlessly integrating with the terminal driver to erase characters on the screen and decrement the buffer.
- **Execution:** Upon pressing `Enter` (\n), the command buffer is evaluated.

## Built-in Commands
Currently, all commands are built-in shell functions, evaluated using standard string utilities (`strcmp`, `strncmp`):
- `help` - Displays a list of available commands.
- `version` - Prints the OS version information.
- `clear` - Clears the VGA text buffer and resets the cursor to the top-left.
- `echo <text>` - Prints the provided text back to the terminal.
