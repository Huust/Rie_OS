#include "global.h"
#include "print.h"
#include "io.h"
#include "interrupt.h"
//descriptor number
#define desc_number (0x81)
#define desc_num_raw 48
//eflag
#define EFLAG_IF 0x00000200
/*
define pic port
ICW1,OCW2,3-->0x20(M)/0xa0(S)
ICW2-4,OCW1-->0x21(M)/0xa1(S)
*/
#define PIC_M_EVEN 0x20
#define PIC_S_EVEN 0xa0
#define PIC_M_ODD 0x21
#define PIC_S_ODD 0xa1

extern void syscall_entry(void);

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
extern uint32_t handler_entry_table[desc_num_raw];

/*-----------------------
idt描述符初始化func
@parameter:中断处理程序段选择子selector
           中断门描述符属性property
@return: void
-----------------------*/
static void idt_desc_init(uint16_t selector,uint16_t property)
{
    for(int i=0;i<desc_num_raw;i++){
        idt[i].handler_addr_offset_low16
         = handler_entry_table[i] & 0x0000FFFF;
        idt[i].handler_addr_offset_high16
         = (handler_entry_table[i] & 0xFFFF0000) >> 16;
         idt[i].handler_selector = selector;
        idt[i].const_byte = 0;
        idt[i].property = property;
    }
}


static void idt_syscall_init(uint16_t selector,uint16_t property)
{
    idt[0x80].handler_addr_offset_low16
    = (uint32_t)syscall_entry & 0x0000FFFF;
    idt[0x80].handler_addr_offset_high16
    = ((uint32_t)syscall_entry & 0xFFFF0000) >> 16;
    idt[0x80].handler_selector = selector;
    idt[0x80].const_byte = 0;
    idt[0x80].property = property;
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
    
    //负责控制8259A的中断屏蔽
    outb(PIC_M_ODD,0xfe);   //主ocw1高6位,最低位全部置1屏蔽
    outb(PIC_S_ODD,0xff);   //从ocw1的8位全部置1屏蔽
    
    rie_puts("pic init\r\n");
}


//存放中断处理函数地址
void* handler_table[desc_number] = {0};
//异常名称
uint8_t* intr_tag[desc_number] = {0};


//简陋的中断处理函数
void intr_handler(uint8_t intr_number)
{
    if ((intr_number == 0x27)||(intr_number == 0x3f)){
        return; //碰到IRQ7或IRQ15,判定为伪中断,不做处理
    }else{
        rie_puts("\r\nintr_number:");
        rie_puti((uint32_t)intr_number);
    }
    if(intr_number == 0xd) {while(1);}
}


/*------------------
function:
将handler_table每个元素设置为函数intr_handler的地址
初始化intr_tag中的异常名称,方便以后调试
-------------------*/
static void exception_init(void)
{
    for(int i=0;i<desc_number;i++){
        //暂时先将全部中断处理函数指向同一个函数
        handler_table[i] = intr_handler;   
    }
    intr_tag[0] = "#DE Divide Error";
    intr_tag[1] = "#DB Debug Exception";
    intr_tag[2] = "NMI Interrupt";
    intr_tag[3] = "#BP Breakpoint Exception";
    intr_tag[4] = "#OF Overflow Exception";
    intr_tag[5] = "#BR BOUND Range Exceeded Exception";
    intr_tag[6] = "#UD Invalid Opcode Exception";
    intr_tag[7] = "#NM Device Not Available Exception";
    intr_tag[8] = "#DF Double Fault Exception";
    intr_tag[9] = "Coprocessor Segment Overrun";
    intr_tag[10] = "#TS Invalid TSS Exception";
    intr_tag[11] = "#NP Segment Not Present";
    intr_tag[12] = "#SS Stack Fault Exception";
    intr_tag[13] = "#GP General Protection Exception";
    intr_tag[14] = "#PF Page-Fault Exception";
    // intr_tag[15] 第15项是intel保留项，未使用
    intr_tag[16] = "#MF x87 FPU Floating-Point Error";
    intr_tag[17] = "#AC Alignment Check Exception";
    intr_tag[18] = "#MC Machine-Check Exception";
    intr_tag[19] = "#XF SIMD Floating-Point Exception";
}

/*intr_handler_register
@function:
    注册中断处理函数
@param:
    irq_num:IRQ中断号
    function:处理函数地址
@notes:
    irq_num是IRQ号而不是中断号，IRQ对应中断号从0x20开始    
*/
void intr_handler_register(uint8_t irq_num,
                            void* function)
{
    handler_table[irq_num + 0x20] = function;
}



//中断相关初始化
void idt_init(void)
{
    idt_desc_init(SELECTOR_K_CODE,IDT_DESC_PROPERTY_DPL0);
    idt_syscall_init(SELECTOR_K_CODE,IDT_DESC_PROPERTY_DPL3);
    exception_init();
    pic_init();
    //描述符加载到lidt中
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    asm volatile("lidt %0" : : "m" (idt_operand));
    rie_puts("idt_init done\r\n");
}

/*-------------------
function:实现开关中断
获取eflag位-->判断中断位是否被开启,获取旧的中断状态
-->enable()和disable()函数,分别用于开关中断
-------------------*/

intr_status old_status = intr_close;

intr_status intr_get_status(void)
{
    //获取eflag的值,存到eflags变量中
    uint32_t eflags = 0;
    asm volatile("pushfl; popl %0" : "=g" (eflags));
    //&运算判断eflags中的if位是否被set
    return (EFLAG_IF&eflags)?intr_open:intr_close;
}



/*rie_intr_enable(disable)
@notes:
    为什么需要old_status这个变量?
    个人感觉这个变量是冗余的.因为用到该变量的三个
    功能函数中涉及到old_status的语句都可以略去
    但是为了防止之后需要用到,暂做保留
    (唯一的作用是,当你需要获取状态时,可以少调用一次get_status()
    函数,仅仅读这个值即可;但似乎不是那么放心)
*/
void rie_intr_enable(void)
{
    intr_status current_status = intr_get_status();
    if(current_status) {old_status = current_status;}
    else{
        old_status = current_status;
        asm volatile("sti");
    }
}


void rie_intr_disable(void)
{
    intr_status current_status = intr_get_status();
    if(!current_status) {old_status = current_status;}
    else{
        old_status = current_status;
        asm volatile("cli" : : : "memory");
    }
}   


/*rie_intr_set
将想要设置的中断开关状态传入函数
*/
void rie_intr_set(intr_status status)
{
    intr_status current_status = intr_get_status();
    old_status = current_status;
    if(status == intr_close){
        asm volatile("cli" : : : "memory");
    }else{  //status == intr_open
        asm volatile("sti");
    }
}