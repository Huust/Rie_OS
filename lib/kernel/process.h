#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "./thread.h"
#include "./memory.h"
#include "./tss.h"
#include "./global.h"

#define PROCESS_VADDR_START 0x8048000   //用户进程所能使用的虚拟内存的起始地址
#define KERNEL_VADDR_START 0xc0000000

//void pt_activate(struct thread_pcb* pthread);

#endif

