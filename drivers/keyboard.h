#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

void keyboard_initialize(void);
char keyboard_getchar(void);
bool keyboard_haskey(void);

#endif
