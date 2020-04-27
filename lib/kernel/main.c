#include "print.h"
int main(void)
{
    rie_putc('r');
    rie_putc('i');
    rie_putc('e');
    rie_putc('o');
    rie_putc('s');
    rie_puts("\r\nkernel");
    while(1);
    return 0;
}