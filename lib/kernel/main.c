#include "print.h"
#include "interrupt.h"
int main(void)
{
    rie_puts("hello");
    rie_putc('2');
    rie_putc('\n');
    rie_putc('1');
    rie_putc('\n');    
    rie_putc('i');
    rie_putc('\n');
    rie_putc('3');    
    rie_putc('m');
    rie_putc('\n');    
    rie_putc('q');
    rie_putc('\n');
    rie_putc('w');    
    rie_putc('\n');
    rie_putc('e');
    rie_putc('\n');
    rie_putc('r');
    rie_putc('w');    
    rie_putc('\n');
        rie_puts("hello");
    rie_putc('e');
    // rie_putc('\r');
    // rie_putc('\n');
    // rie_putc('\r');
    // rie_putc('\n');
    // rie_puts("hello");
    //rie_puts("y");
    // rie_puts("this is test 4\r\n");
    // rie_puts("this is test 5\r\n");
    // rie_puts("this is test 6\r\n");
    // rie_puts("this is test 7\r\n");
    // rie_puts("this is test 8\r\n");
    // rie_puts("this is test 9\r\n");
    //rie_puts("this is test 10\r\n");
    while(1);
    rie_puts("rieos kernel\r\n");
    idt_init();
    asm volatile ("sti");   //打开全局中断
    while(1);
    return 0;
}