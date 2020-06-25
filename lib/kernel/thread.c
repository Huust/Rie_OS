#include "./thread.h"

//调用switch_to.asm中的switch_to()函数
extern void switch_to(struct thread_pcb*, struct thread_pcb*);

//宏函数，通过结构体成员地址反推结构体地址
#define offset(struct_type, member)     \
(uint32_t)(&((struct_type*)0)->member)

#define elem2pcb(struct_type, member, offset)      \
(struct_type*)((uint32_t)member - offset)


struct thread_pcb* main_thread;
struct list* all_list;
struct list* ready_list;

void kernel_thread(thread_func* function, 
                    void* arg)
{
    function(arg);
}


/*

*/
static void pcb_register(struct thread_pcb* pthread,
                const char* name, 
                uint8_t priority)
{
    rie_memset(pthread, 0, sizeof(struct thread_pcb));
    rie_memcpy(name,pthread->name,sizeof(name));
    pthread->prior = priority;
    pthread->bound_detect = 0x20000803;
    pthread->tick = 32 - priority;  //note:prior小代表优先级大
    pthread->all_tick = 0;
    pthread->vaddr = NULL;
    //fixme:对于main thread这个stack_ptr是不是太高了
    pthread->stack_ptr = (uint32_t)pthread + PAGE_SIZE;
    if(pthread == main_thread)
        pthread->status = TASK_RUNNING;
    else
        pthread->status = TASK_READY;
}


/*

*/
static void thread_create(struct thread_pcb * pthread,
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

    //第一次被加载线程，初始化四个寄存器的值
    thread_stack->ebp = 0;
    thread_stack->ebx = 0;
    thread_stack->edi = 0;
    thread_stack->esi = 0;

    thread_stack->func_arg = arg;
    thread_stack->function = function;
}


/*
@param:

*/
void thread_start(const char* name, 
                thread_func* function, 
                void* arg, 
                uint8_t priority)
{
    //内核物理内存中分配一页用来创建PCB
    struct thread_pcb * thread = get_kernel_page(1);
    rie_memset(thread, 0, sizeof(struct thread_pcb));

    //PCB初始化工作交给thread_init()   thread_create()
    pcb_register(thread, name, priority);
    thread_create(thread, function, arg);


    //ASSERT是多余的，不过以防万一还是加上了
    ASSERT(!elem_search(all_list, &thread->all_list_elem));
    list_append(all_list, &thread->all_list_elem);

    ASSERT(!elem_search(ready_list, &thread->ready_list_elem));
    list_append(all_list, &thread->ready_list_elem);

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
    // rie_puts("\r\n");
    // rie_puti(esp);
    return (struct thread_pcb*)(esp & 0xfffff000);
}


static void main_thread_register(void)
{
    main_thread = get_running_thread();
    //获得main thread的pcb后填充pcb内容
    pcb_register(main_thread,"main",1);
    ASSERT(!elem_search(all_list, &main_thread->all_list_elem));
    list_append(all_list, &main_thread->all_list_elem);
}


void schedule(void)
{
    rie_puts("schedule!!!!!!");
    while(1);
    ASSERT(intr_close==get_status());
    struct thread_pcb* cur_thread = get_running_thread();
    //fixme:若只有main线程，则第一次调度时会断言失败
    ASSERT(!list_empty(ready_list));
    struct list_element* next_elem = list_pop(ready_list);
    
    if(cur_thread->status == TASK_RUNNING){
        cur_thread->tick = 32 - cur_thread->prior;
        cur_thread->status = TASK_READY;
        
        list_append(ready_list, &cur_thread->ready_list_elem);
    }else{
        //todo:非时间片原因造成的schedule；暂时不做讨论
    }

    //通过next_elem成员地址反推其结构体地址
    struct thread_pcb* next_thread = elem2pcb(struct thread_pcb, 
                                            next_elem, 
                            offset(struct thread_pcb, ready_list_elem));

    next_thread->status = TASK_RUNNING;

    switch_to(cur_thread, next_thread);
}

/*

*/
void thread_init(void)
{
    list_init(all_list);
    list_init(ready_list);
    main_thread_register();
}