#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
void idt_init(void);
void intr_handler(uint8_t intr_number);
void rie_intr_enable(void);
void rie_intr_disable(void);
//枚举类型,存放中断状态
typedef enum {
    intr_open = 0,
    intr_close
}intr_status;

#endif