#ifndef E1000_H
#define E1000_H

#include <stdint.h>

void e1000_initialize(void);
void e1000_get_mac(uint8_t* mac);
int e1000_is_found(void);

#endif
