#include "./thread.h"

//调用switch_to.asm中的switch_to()函数
extern void switch_to(struct thread_pcb*, struct thread_pcb*);

struct thread_pcb* main_thread;
struct list all_list;
struct list ready_list;


/*kernel_thread
@function:
    由thread_stack中的eip指针指向的同函数类型
*/
void kernel_thread(thread_func* function, 
                    void* arg)
{
    /*  
        某个线程第一次被调度上去会执行kernel_thread()；
        因为执行流跑到这里了所以此时中断是关闭的
        所以需要打开中断，否则即将运行的线程就不能被时钟中断调度下来了 
    */
    rie_intr_enable();
    function(arg);
}


/*pcb_enroll
@function:
    pcb登记:填充线程pcb的内容
*/
void pcb_enroll(struct thread_pcb* pthread,
                const char* name, 
                uint8_t priority)
{
    rie_memset(pthread, 0, sizeof(struct thread_pcb));
    rie_memcpy(name,pthread->name,sizeof(name));
    pthread->prior = priority;
    pthread->bound_detect = 0x20000803;
    pthread->tick = 32 - priority;  //prior小代表优先级大
    pthread->all_tick = 0;
    pthread->pt_vaddr = NULL;
    pthread->stack_ptr = (uint32_t)pthread + PAGE_SIZE;
    if(pthread == main_thread)
        pthread->status = TASK_RUNNING;
    else
        pthread->status = TASK_READY;
}


/*thread_create
@function:
    在已经分配好的1页线程堆内存中,进一步划分intr_stack,thread_stack
    并且完善线程栈
*/
void thread_create(struct thread_pcb * pthread,
                    thread_func* function,
                    void* arg)
{
    /*修改栈顶指针的值(使指针指向thread_stack栈底)*/
    pthread->stack_ptr -= sizeof(struct intr_stack);
    pthread->stack_ptr -= sizeof(struct thread_stack);
    struct thread_stack* thread_stack = (struct thread_stack*)(pthread->stack_ptr);

    /*thread_stack结构体中数据初始化工作*/

    //eip函数指针指向统一调用线程的那个函数，方便之后ret跳转
    thread_stack->eip = kernel_thread;

    //第一次被加载的线程，初始化四个寄存器的值以供pop(switch_to.asm line19)使用
    thread_stack->ebp = 0;
    thread_stack->ebx = 0;
    thread_stack->edi = 0;
    thread_stack->esi = 0;

    thread_stack->func_arg = arg;
    thread_stack->function = function;
}


/*thread_start
@function:
    线程启动:包括 pcb注册 | 线程栈分配 | 线程信息加入线程队列以供
    调度器使用
*/
void thread_start(const char* name, 
                thread_func* function, 
                void* arg, 
                uint8_t priority)
{
    //内核物理内存中分配一页用来创建PCB
    struct thread_pcb * thread = get_kernel_page(1);

    //PCB初始化工作交给pcb_enroll()   thread_create()
    pcb_enroll(thread, name, priority);
    thread_create(thread, function, arg);


    //ASSERT是多余的，不过以防万一还是加上了
    ASSERT(!elem_search(&all_list, &thread->all_list_elem));
    list_append(&all_list, &thread->all_list_elem);

    ASSERT(!elem_search(&ready_list, &thread->ready_list_elem));
    list_append(&ready_list, &thread->ready_list_elem);

    //内联汇编部分负责跳转到线程执行（因为涉及到修改eip）
    //同时将esp的值修改为thread stack栈底值
    // asm volatile ("movl %0, %%esp; \
    // pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; \
    // ret": : "g" (thread->stack_ptr) : "memory");
}


struct thread_pcb* get_running_thread(void)
{
    uint32_t esp;
    asm ("mov %%esp, %0" : "=g" (esp));
    return (struct thread_pcb*)(esp & 0xfffff000);
}


static void main_thread_register(void)
{
    main_thread = get_running_thread();
    //获得main thread的pcb后填充pcb内容
    pcb_enroll(main_thread,"main",10);
    ASSERT(!elem_search(&all_list, &main_thread->all_list_elem));
    list_append(&all_list, &main_thread->all_list_elem);
}


void schedule(void)
{
    ASSERT(intr_close==intr_get_status());
    struct thread_pcb* cur_thread = get_running_thread();
    
    if(cur_thread->status == TASK_RUNNING){
        cur_thread->tick = 32 - cur_thread->prior;
        cur_thread->status = TASK_READY;
        
        list_append(&ready_list, &cur_thread->ready_list_elem);
    }else{
        /* 非时间片调度， 比如某个thread因为block而调用schedule() */
        /* 线程无法通过时间片自动被调度执行，所以不能append到ready_list */
    }

    struct list_element* next_elem = list_pop(&ready_list);
    //通过next_elem成员地址反推其结构体地址
    struct thread_pcb* next_thread = elem2pcb(struct thread_pcb, \
                                            next_elem, \
                            offset(struct thread_pcb, ready_list_elem));

    next_thread->status = TASK_RUNNING;

    //pt_activate(next_thread);

    switch_to(cur_thread, next_thread);
}


void thread_block(enum task_status status)
{
    ASSERT((status == TASK_BLOCK) || (status == TASK_WAITING)   \
            || (status == TASK_SUSPEND));
    //进入临界区
    intr_status old_status = intr_get_status();
    rie_intr_disable();

    struct thread_pcb* current_thread = get_running_thread();
    current_thread->status = status;

    schedule();
    //退出临界区
    rie_intr_set(old_status);
}


void thread_unblock(struct thread_pcb* pthread)
{
    intr_status old_status = intr_get_status();
    rie_intr_disable();

    //断言:状态判断;是否存在于ready_list中
    ASSERT((pthread->status != TASK_READY)    \
    && (!elem_search(&ready_list, &pthread->ready_list_elem)));
    list_push(&ready_list, &pthread->ready_list_elem);
    pthread->status = TASK_READY;

    rie_intr_set(old_status);
}
/*

*/
void thread_init(void)
{
    list_init(&all_list);
    list_init(&ready_list);
    main_thread_register();
}