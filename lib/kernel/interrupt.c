#include "stdint.h"
#include "global.h"
#include "print.h"
#include "io.h"
//descriptor number
#define desc_number 33
//define pic port
/*
ICW1,OCW2,3-->0x20(M)/0xa0(S)
ICW2-4,OCW1-->0x21(M)/0xa1(S)
*/
#define PIC_M_EVEN 0x20
#define PIC_S_EVEN 0xa0
#define PIC_M_ODD 0x21
#define PIC_S_ODD 0xa1

//定义描述符结构体
typedef struct descriptor
{
    uint16_t handler_addr_offset_low16;
    uint16_t handler_selector;
    uint8_t const_byte;
    uint8_t property;
    uint16_t handler_addr_offset_high16;
}idt_desc;

//创建结构体数组存放全部中断描述符
static idt_desc idt[desc_number] = {0};
//kernel.asm中的中断处理程序入口地址数组
extern void* handler_entry_table[desc_number];


/*-----------------------
idt描述符初始化func
@parameter:中断处理程序段选择子selector
           中断门描述符属性property
@return: void
-----------------------*/
static void idt_desc_init(uint16_t selector,uint16_t property)
{
    for(int i=0;i<desc_number;i++){
        idt[i].handler_addr_offset_low16
         = (uint32_t)handler_entry_table[i] & 0x0000FFFF;
        idt[i].handler_addr_offset_high16
         = ((uint32_t)handler_entry_table[i] & 0xFFFF0000) >> 16;
         idt[i].handler_selector = selector;
        idt[i].const_byte = 0;
        idt[i].property = property;
    }
    rie_puts("idt descriptor init\r\n");
}

//中断控制器初始化
static void pic_init(void)
{
    //主片ICW1-4
    outb(PIC_M_EVEN,0x11);
    outb(PIC_M_ODD,0x20);
    outb(PIC_M_ODD,0x04);
    outb(PIC_M_ODD,0x01);
    //从片ICW1-4
    outb(PIC_S_EVEN,0x11);
    outb(PIC_S_ODD,0x28);    
    outb(PIC_S_ODD,0x02);
    outb(PIC_S_ODD,0x01);
    
    //屏蔽R1-R15的中断请求
    outb(PIC_M_ODD,0xfe);   //主ocw1高7位全部置1屏蔽
    outb(PIC_S_ODD,0xff);   //从ocw1的8位全部置1屏蔽
    
    rie_puts("pic init\r\n");
}

void idt_init(void)
{
    idt_desc_init(SELECTOR_CODE,IDT_DESC_PROPERTY_DPL0);
    pic_init();
    //描述符加载到lidt中
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    asm volatile("lidt %0" : : "m" (idt_operand));
    rie_puts("idt_init done\r\n");
}