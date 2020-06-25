#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include "stdint.h"

//枚举类型,存放中断状态
typedef enum {
    intr_close = 0,
    intr_open
}intr_status;

void idt_init(void);
void intr_handler(uint8_t intr_number);
void rie_intr_enable(void);
void rie_intr_disable(void);
void intr_handler_register(uint8_t irq_num,
                            void* function);
intr_status get_status(void);
void rie_intr_set(intr_status status);
#endif