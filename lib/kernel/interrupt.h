#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
static void idt_desc_init(uint16_t selector,uint16_t property);
static void pic_init(void);
void idt_init(void);
#endif