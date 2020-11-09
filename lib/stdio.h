#ifndef _STDIO_H_
#define _STDIO_H_

#include "./stdint.h"
#include "./string.h"
#include "./kernel/syscall.h"
#include "./kernel/debug.h"

typedef char* va_list;

void itoa(int32_t integer, char** const buf, uint8_t base);
uint32_t mySprintf(char* const buf, const char* format, va_list ap);
int printf(const char* format, ...);
#endif