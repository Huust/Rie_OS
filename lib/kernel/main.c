#include "print.h"
#include "interrupt.h"
#include "debug.h"
#include "memory.h"
int main(void)
{
    
    rie_puts("rieos kernel\r\n");
    //idt_init();
    //ASSERT(2==3);
    //asm volatile ("sti");   //打开全局中断
    void mem_struct_init(,);
    void* get_kernel_page();
    while(1);
    return 0;
}