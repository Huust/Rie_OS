#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "../stdint.h"
#include "./thread.h"
#include "./console.h"
#include "../string.h"

#define syscall_number 32

enum SYS_NUMBER{
    SYS_GETPID = 0,
    SYS_WRITE,
};

void syscall_init(void);
uint16_t get_pid(void);
#endif