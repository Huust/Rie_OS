#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "./print.h"
#include "./sync.h"

void console_init(void);
void console_acquire(void);
void console_release(void);
void console_puts(char* str);
void console_putc(uint8_t char);
void console_puti(uint32_t num);
#endif