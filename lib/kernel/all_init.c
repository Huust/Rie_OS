#include "./all_init.h"

void all_init(void)
{
    idt_init();
    tss_init();
    console_init();     //对应的锁：console_lock
    //keyboard_init();
    timer_init();
    mem_struct_init(0x2000000,256);
    thread_init();
}