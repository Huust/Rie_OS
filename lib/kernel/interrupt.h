#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
void idt_init(void);
void intr_handler(uint8_t intr_number);
#endif