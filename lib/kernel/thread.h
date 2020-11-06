#ifndef _THREAD_H_
#define _THREAD_H_

#include "./debug.h"
#include "./memory.h"
#include "./bitmap.h"
#include "../string.h"
#include "./list.h"
#include "./interrupt.h"


/*定义一个通用的函数类型
方便后期广泛使用函数指针
*/


//宏函数，通过结构体成员地址反推结构体地址
#define offset(struct_type, member)     \
(uint32_t)(&((struct_type*)0)->member)      //这边仅仅是读操作,没有写所以不涉及非法访问


#define elem2pcb(struct_type, member, offset)      \
(struct_type*)((uint32_t)member - offset)


typedef void thread_func(void* arg);
/*task_status
线程状态（目前仅有运行态和就绪态）
*/
enum task_status{
    TASK_RUNNING = 0,
    TASK_READY,
    TASK_BLOCK,
    TASK_WAITING,
    TASK_SUSPEND
};


/*定义中断栈intr_stack
@function:
    (×)线程运行中，被中断等异常打断时，寄存器值压栈到该结构体内
    (√)内核线程->用户进程 时，需要使用中断实现特权转移，为了复用kernel.asm
    中iretd执行前的pop操作，所以使用intr_stack存放被pop出来的数据；同时利
    用这次pop操作，为即将执行的用户进程提供新的寄存器环境
*/
struct intr_stack{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t gs;
    uint32_t err_code;
    void (*eip) (void);     //参数为空，返回值为空的函数指针
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

/*定义线程栈thread_stack
@function:
    线程之间正常调度时，保存线程的一些寄存器值；
    同时保存了执行线程内容的函数的入口地址，方便跳转
    thread_stack存放 内核线程 调度时需要的值；
    intr_stack存放 用户进程 调度时需要的值
*/
struct thread_stack{
    uint32_t esi;
    uint32_t edi;
    uint32_t ebx;
    uint32_t ebp;
    void (*eip) (thread_func* function,
                    void* func_arg);

    void* nonsense;     //定义一个空指针用来占用一个栈帧

    //note:注意下面两个变量在结构体的前后顺序，因为涉及参数传递顺序
    thread_func* function;
    void* func_arg;
};



/*定义PCB(process control block)
@function:
    保存线程信息，如name priority stack_pointer等
*/
struct thread_pcb{
    uint32_t stack_ptr;     //栈顶指针
                            //pcb_enroll()中：初始化为申请得到的
                            //页地址 + page_size

    thread_func* function;
    void* arg;
    char name[16];
    uint8_t prior;
    enum task_status status;

    uint8_t tick;           //时间片
    uint32_t all_tick;      //该线程运行的总时长

    uint32_t* pd_vaddr;         //页目录表的地址;用户进程被调度前将
                                //该成员值将被载入CR3实现页表切换

    struct virtual_pool user_heap;  //如果是用户进程，则在该pcb中
                                    //有负责管理堆内存的结构体
    
    struct list_element all_list_elem;

    struct list_element ready_list_elem;

    uint32_t bound_detect;
};
void thread_start(const char* name, 
                thread_func* function, 
                void* arg, 
                uint8_t priority);

void thread_create(struct thread_pcb * pthread,
                    thread_func* function,
                    void* arg);

void pcb_enroll(struct thread_pcb* pthread,
                const char* name, 
                uint8_t priority);

struct thread_pcb* get_running_thread(void);
void schedule(void);
void thread_block(enum task_status status);
void thread_unblock(struct thread_pcb* thread);
void thread_init(void);

#endif