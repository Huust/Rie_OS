#include "./process.h"

extern struct list all_list;
extern struct list ready_list;
extern void intr_exit(void);
extern void update_tss_esp0(struct thread_pcb* pthread);

/* 进程执行前的初始化工作 
    该函数被thread.c中的kernel_thread()调用
@param:
    proc_func进程的函数体
*/
static void process_exec(void* proc_func)
{
    struct thread_pcb* cur_thread = get_running_thread();
    
    /*pcb的栈指针从线程栈切换到进程栈，因为该函数执行前是对
        线程的初始化，其中已经初始化栈指针指向thread_stack栈底,
        所以执行下面代码后stack_ptr将指向intr_stack栈底
    */
    cur_thread->stack_ptr += sizeof(struct thread_stack);

    struct intr_stack*  proc_stack = (struct intr_stack*)(cur_thread->stack_ptr);
    //proc_stack是intr_stack的栈底,接下来对这个栈进行初始化
    proc_stack->edi = proc_stack->esi = proc_stack->eax = proc_stack->ebx \
     = proc_stack->ecx = proc_stack->edx = 0;


    proc_stack->gs = 0;

    proc_stack->ds = proc_stack->es = proc_stack->fs = SELECTOR_U_DATA;

    proc_stack->ss = SELECTOR_U_STACK;

    proc_stack->cs = SELECTOR_U_CODE;
    proc_stack->eip = proc_func;    /* 要求proc_func返回值和参数必须都为void */

    proc_stack->esp_dummy = proc_stack->ebp = 0;
    /*todo*/
    uint32_t proc_esp = (uint32_t)get_a_concrete_page(APP_USER, USER_STACK3_VADDR) + PAGE_SIZE;
    ASSERT(proc_esp != PAGE_SIZE);
    proc_stack->esp = (void*)proc_esp;
    proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);

    /* 使用内联汇编，第一步：将esp设置为proc_stack地址
    第二步：调用kernel.asm中的intr_exit函数*/
    asm volatile ("movl %0, %%esp; jmp intr_exit" \
    : : "g" (proc_stack) : "memory");

}

/* 页表激活 */
void pt_activate(struct thread_pcb* pthread)
{
    uint32_t pt_paddr = 0x100000;
    if (pthread->pt_vaddr != NULL) {
        pt_paddr = addr_v2p((uint32_t)(pthread->pt_vaddr));
    }

    
    //内联汇编将新的页表物理地址载入CR3
    asm volatile ("movl %0, %%cr3" : : "r" (pt_paddr) : "memory");
}

/* 进程激活:页表激活 + 更新esp0 
    若下一个执行流是线程，则页表保持不变，esp0也不需要更改
    若为进程，则cr3更改进程pcb中的页目录表地址，esp0修改为intr_stack栈顶
    再强调一下，传入的pthread是next_thread而不是cur_thread
*/
void process_activate(struct thread_pcb* pthread)
{
    pt_activate(pthread);
    
    if (pthread->pt_vaddr != NULL) {
        update_tss_esp0(pthread);
    }

}

/* 为用户进程创建页目录表,并且写入内核高1GB映射的内容
    进程共享内核
    @return:
        pd_vaddr页目录表的虚拟地址
*/
uint32_t create_page_dir()
{
    uint32_t pd_vaddr = (uint32_t)get_kernel_page(1);

    /*因为执行这行代码所处的环境依然是内核态，所以是将
      内核页目录表中记载高1GB的映射内容（内核占用）copy到用户进程的页目录表
      依然是通过欺骗CPU达到访问pde的效果
    */
    rie_memcpy((void*)(0xfffff000 + 0x300*4), (void*)(pd_vaddr + 0x300*4), \
    1024);

    uint32_t pd_paddr = addr_v2p((uint32_t)pd_vaddr);

    //页目录表的最后4B依然存放其首地址
    *((uint32_t*)(pd_vaddr + 1023*4)) = pd_paddr | PG_US_U | PG_RW_W | PG_P_1;
    
    return pd_vaddr;
}


/* 创建进程的内存位图 */
void create_process_bitmap(struct thread_pcb* pthread)
{
    /* 规定进程所使用的虚拟地址从PROCESS_VADDR_START开始 */
    /* bitmap_page_num:映射一段虚拟内存需要的位图所占的页框数 */
    /* 计算公式：该页框数 = 内存size/4k/8/4k */
    /* 加1是因为：计算所的非整数，故向上取整*/
    uint32_t bitmap_page_num = (KERNEL_VADDR_START - \
    PROCESS_VADDR_START)/(4*1024)/8/(4*1024) + 1;

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


/*process_start
@param:
    proc_func:进程函数
    name:函数名
    priority:优先级
*/
void process_start(const char* name, \
                    void* proc_func, uint8_t priority)
{
    struct thread_pcb* proc = get_kernel_page(1);

    pcb_enroll(proc, name, priority);
    proc->pt_vaddr = (uint32_t*)create_page_dir();
    /* kernel_thread()中调用process_exec()，
        process_exec()中调用proc_func() */
    thread_create(proc, process_exec, proc_func);
    create_process_bitmap(proc);

    intr_status old_status = intr_get_status();

    ASSERT(!elem_search(&all_list, &proc->all_list_elem));
    list_append(&all_list, &proc->all_list_elem);

    ASSERT(!elem_search(&ready_list, &proc->ready_list_elem));
    list_append(&ready_list, &proc->ready_list_elem);

    rie_intr_set(old_status);
}

