#ifndef _RIEPRINT_H_
#define _RIEPRINT_H_
#include "../stdint.h"
void rie_putc(uint8_t rie_char);
void rie_puts(uint8_t* rie_str);
void rie_puti(uint32_t rie_int);
#endif