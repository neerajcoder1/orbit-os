#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memset(void* ptr, int value, size_t num);
void* memcpy(void* destination, const void* source, size_t num);
size_t strlen(const char* str);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char* itoa(int value, char* str, int base);
char* strcpy(char* dest, const char* src);

#endif
