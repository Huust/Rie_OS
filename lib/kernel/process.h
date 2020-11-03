#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "./thread.h"
#include "./memory.h"
#include "./global.h"

#define PROCESS_VADDR_START 0x8048000   //用户进程所能使用的虚拟内存的起始地址
#define KERNEL_VADDR_START 0xc0000000   //区别memory.c中的KERNEL_HEAP_START
#define USER_STACK3_VADDR (0xc0000000 - 0x1000)

void process_start(const char* name, \
                    void* proc_func, uint8_t priority);
#endif

