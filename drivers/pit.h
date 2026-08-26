#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_initialize(uint32_t frequency);
uint32_t pit_get_ticks(void);

#endif
