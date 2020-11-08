#include "./syscall.h"
/* _syscall0 
@param:
    NUMBER 子功能号
@return:
    retval 系统调用的返回值

@notes:
    0代表没有额外参数传入，只有子功能号
    =a代表返回值用eax保存
    a代表子功能号用eax保存
    最后一行写了retval; 表示整个函数结束后返回retval值（返回值存在eax中）
    含参数传入的，bcd代表存放到ebx，ecx，edx
*/
#define _syscall0(NUMBER) ({    \
int retval; \
asm volatile (  \
"int $0x80" \
: "=a" (retval) \
: "a" (NUMBER)  \
: "memory"  \
);  \
retval; \
})

#define _syscall1(NUMBER, ARG1) ({  \
int retval; \
asm volatile (  \
"int $0x80" \
: "=a" (retval) \
: "a" (NUMBER) "b" (ARG1)   \
: "memory"  \
);  \
retval; \
})

#define _syscall2(NUMBER, ARG1, ARG2) ({    \
int retval; \
asm volatile (  \
"int $0x80" \
: "=a" (retval) \
: "a" (NUMBER) "b" (ARG1) "c" (ARG2)    \
: "memory"  \
);  \
retval; \
})

#define _syscall3(NUMBER, ARG1, ARG2, ARG3) ({  \
int retval; \
asm volatile (  \
"int $0x80" \
: "=a" (retval) \
: "a" (NUMBER) "b" (ARG1) "c" (ARG2) "d" (ARG3) \
: "memory"  \
);  \
retval; \
})

/* 定义系统调用表 */
void* syscall_table[syscall_number] = {0};

void syscall_register(enum SYS_NUMBER syscall_num, void* syscall_func)
{
    syscall_table[syscall_num] = syscall_func;
}


/*------------------------------------------------------*/
/* getpid的底层实现：返回当前运行线程/进程的pid值 */
static uint16_t sys_getpid(void)
{
    return (get_running_thread()->pid);
}


/*------------------------------------------------------*/
/* 初始化 */
void syscall_init(void)
{
    syscall_register(SYS_GETPID, sys_getpid);
}


/*------------------------------------------------------*/
/* 提供给用户的系统调用接口 */
uint16_t get_pid(void)
{
    return (_syscall0(SYS_GETPID));
}
