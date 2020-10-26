#include "./process.h"


void process_init(void)
{
    struct thread_pcb* cur_thread = get_running_thread();
    
    //pcb的栈指针从线程栈切换到进程栈
    cur_thread->stack_ptr += sizeof(struct thread_stack);

    struct intr_stack*  proc_stack= (struct intr_stack*)(cur_thread->stack_ptr);
    //proc_stack是intr_stack的栈底,接下来对这个栈进行初始化
    //todo:因为还不清楚这个栈的内容何时被使用,因此先不填充

}

/* 页表激活 */
void pt_activate(struct thread_pcb* pthread)
{
    uint32_t pt_paddr = 0x100000;
    if (pthread->pt_vaddr != NULL) {
        pt_paddr = addr_v2p(pthread->pt_vaddr);
    }

    //内联汇编将新的页表物理地址载入CR3
    asm volatile ("movl %0, %%cr3" : : "r" (pt_paddr) : "memory");
}

/* 进程激活:页表激活 + 更新esp0 */
void process_activate(struct thread_pcb* pthread)
{
    pt_activate(pthread);
    
    if (pthread->pt_vaddr != NULL) {
        update_tss_esp0(pthread);
    }

}

/* 为用户进程创建页目录表,并且写入内核高1GB映射的内容 */
/* 进程共享内核 */
uint32_t create_page_dir()
{
    void* pd_vaddr = get_kernel_page(1);

    /*因为执行这行代码所处的环境依然是内核态，所以是将
      内核页目录表中记载高1GB的映射内容（内核占用）copy到用户进程的页目录表
      依然是通过欺骗CPU达到访问pde的效果
    */
    rie_memcpy(0xfffff000 + 0x300*4, pd_vaddr + 0x300*4, 1024);

    uint32_t pd_paddr = addr_v2p((uint32_t)pd_vaddr);

    //页目录表的最后4B依然存放其首地址
    &(uint32_t*)(pd_vaddr + 1023*4) = pd_paddr | PG_US_U | PG_RW_W | PG_P_1;
    
    return (uint32_t)pd_vaddr;
}

/* 创建进程的内存位图 */
void create_process_bitmap(struct thread_pcb* pthread)
{
    /* 规定进程所使用的虚拟地址从PROCESS_VADDR_START开始 */
    /* bitmap_page_num:映射一段虚拟内存需要的位图所占的页框数 */
    /* 计算公式：该页框数 = 内存size/4k/8/4k */
    /* 加1是因为：计算所的非整数，故向上取整*/
    uint32_t bitmap_page_num = (PROCESS_VADDR_START - \
    KERNEL_VADDR_START)/(4*1024)/8/(4*1024) + 1;

    /* 向kernel申请该bitmap所需的内存 */
    pthread->user_heap.pool_bitmap.bitmap_set = \
    get_kernel_page(bitmap_page_num);

    /* bitmap相关初始化 */
    pthread->user_heap.pool_bitmap.bitmap_len = \
    bitmap_page_num * 4*1024;
    pthread->user_heap.pool_start = PROCESS_VADDR_START;

    bitmap_init(pthread->user_heap.pool_bitmap);

    return;
}


/* 对struct thread_pcb 结构体成员变量赋值，完成线程pcb内容 */
void process_start(void* func, void* arg, \
                    const char* name, uint8_t priority)
{
    struct thread_pcb* process = get_kernel_page(1);

    pcb_enroll(process, name, priority);
    process->pt_vaddr = (uint32_t*)create_page_dir();
    thread_create(process, func, arg);
    create_process_bitmap(process);

    ASSERT(!elem_search(&all_list, &process->all_list_elem));
    list_append(&all_list, &process->all_list_elem);

    ASSERT(!elem_search(&ready_list, &process->ready_list_elem));
    list_append(&ready_list, &process->ready_list_elem);
}
