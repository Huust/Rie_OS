#include "./keyboard.h"

#define KBD_BUF_PORT 0x60
#define KBD_IRQ 0x01

static void kbd_intr_handler(uint8_t intr_num)
{
    rie_putc('h');
    inb(KBD_BUF_PORT);      //没有传递返回值,但因为是读端口所以存放在了ax寄存器中
}

void keyboard_init()
{
    intr_handler_register(KBD_IRQ, kbd_intr_handler);
    rie_puts("keyboard init done\r\n");
}
