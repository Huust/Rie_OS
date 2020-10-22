#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "./thread.h"
#include "./memory.h"
#include "./tss.h"

#define PROCESS_VADDR_START 0x8048000
#define KERNEL_VADDR_START 0xc0000000

void pt_activate(struct thread_pcb* pthread);

#endif

