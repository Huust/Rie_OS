/* 有了lock功能之后，任何公共资源都可以上锁，确保线程之间互斥访问 */
/* 比如console.c */

#ifndef _SYNC_H_
#define _SYNC_H_
#include "./list.h"
#include "./thread.h"
#include "./interrupt.h"

struct semaphore {
    uint8_t value;
    struct list wait_list;
};

struct lock {
    struct thread_pcb* owner;
    uint32_t repeat_num;    //持有者反复持有次数
                            //防止线程多次执行获取锁的函数，导致信号量减到0以下
    struct semaphore sem;
};


void lock_init(struct lock* plock);
void lock_acquire(struct lock* plock);
void lock_release(struct lock* plock);

#endif