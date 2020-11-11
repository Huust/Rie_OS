#ifndef _STDIO_H_
#define _STDIO_H_

#include "./stdint.h"
#include "./string.h"
#include "./kernel/syscall.h"
#include "./kernel/debug.h"

typedef char* va_list;

static void itoa(uint32_t value, char** buf_ptr_addr, uint8_t base);
uint32_t vsprintf(char* str, const char* format, va_list ap);
uint32_t printf(const char* format, ...);
#endif