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
    struct semaphore sem;
};
#endif
