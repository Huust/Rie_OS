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
    mem_struct_init(0x2000000,256);  
    uint32_t vaddr = (uint32_t)get_kernel_page(3);
    rie_puts("hello");
    while(1);
    return 0;
}