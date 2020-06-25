#include "./all_init.h"

void all_init(void)
{
    idt_init();
    timer_init();
    mem_struct_init(0x2000000,256);
    thread_init();
}