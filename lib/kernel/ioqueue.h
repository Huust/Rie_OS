#ifndef _IOQUEUE_H_
#define _IOQUEUE_H_

#include "./sync.h"
#include "./thread.h"
#include "../string.h"

struct ioqueue{
    struct lock lock;
    int8_t buffer[64];      //
    int32_t head_idx;
    int32_t tail_idx;
    struct thread_pcb* producer;
    struct thread_pcb* consumer;
};

#endif