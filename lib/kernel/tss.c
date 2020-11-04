#include "./tss.h"

static struct tss tss;


/* 更新tss的esp0,从R3向R0调度时调用此函数,
    esp0指向的地址即为R0线程PCB的intr_stack(中断栈) */
void update_tss_esp0(struct thread_pcb* pthread)
{
    tss.esp0 = (uint32_t*)((uint32_t)pthread + PAGE_SIZE);
}


/* 构建gdt的描述符 
@param:
    desc_addr:段基址
    limit:段界限
    attr_low:属性位低四位
    attr_high:属性位高四位

@return:返回构建好的gdt结构体；可以直接赋值到相应GDT表位置
*/
static struct gdt_desc make_gdt_desc(uint32_t* desc_addr, 
                                    uint32_t limit, 
                                    uint8_t attr_low, 
                                    uint8_t attr_high)
{
    uint32_t desc_base = (uint32_t)desc_addr;
    struct gdt_desc desc;
    desc.limit_low_word = (uint16_t)(limit & 0x0000ffff);
    desc.base_low_word = (uint16_t)(desc_base & 0x0000ffff);
    desc.base_mid_byte = (uint8_t)((desc_base & 0x00ff0000) >> 16);
    desc.attr_low_byte = attr_low;
    desc.limit_high_attr_high = \
    (uint8_t)((limit & 0x000f0000) >> 16) + attr_high;
    desc.base_high_byte = (uint8_t)(desc_base >> 24);
    return desc;
}


/* 初始化tss描述符，用于用户进程的DPL为3的目标段描述符*/
void tss_init()
{
    rie_memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_K_STACK;
    tss.io_base = sizeof(tss);  //io位图设为tss大小代表没有io位图

    /* 在 gdt 中添加 dpl 为 0 的 TSS 描述符 */
    /* tss段描述符的段基址就是tss结构体的地址 */
    *((struct gdt_desc*)0xc0000920) = make_gdt_desc((uint32_t*)&tss, 
                                                    sizeof(tss) - 1, 
                                                    TSS_ATTR_LOW,
                                                    TSS_ATTR_HIGH);

    /* 在 gdt 中添加 dpl 为 3 的数据段和代码段描述符 */
    *((struct gdt_desc*)0xc0000928) = make_gdt_desc((uint32_t*)0,
                                                    0xfffff, 
                                                    GDT_CODE_ATTR_LOW_DPL3, 
                                                    GDT_ATTR_HIGH);
    *((struct gdt_desc*)0xc0000930) = make_gdt_desc((uint32_t*)0,
                                                    0xfffff, 
                                                    GDT_DATA_ATTR_LOW_DPL3, 
                                                    GDT_ATTR_HIGH);


    /* gdt 16 位的 limit 32 位的段基址 */
    uint64_t gdt_operand = \
    ((8 * 7 - 1) | ((uint64_t)(uint32_t)0xc0000900 << 16));
    
    asm volatile ("lgdt %0" : : "m" (gdt_operand));
    asm volatile ("ltr %w0" : : "r" (SELECTOR_TSS));
    rie_puts("tss_init and ltr done\n");
}