#include "print.h"
#include "interrupt.h"
int main(void)
{
    rie_puts("rieos kernel\r\n");
    idt_init();
    asm volatile ("sti");   //打开全局中断
    while(1);
    return 0;
}