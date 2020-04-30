#ifndef _LIB_IO_H_
#define _LIB_IO_H_
#include "stdint.h"
/*
将内联调用写在头文件的原因:
    static + inline使得函数调用没有压栈等一系列的处理,而是
直接原地展开,加快了执行速度
    由于static的限制使得函数体必须也写在header中,但这样
    带来的坏处就是每个包含io.h的文件编译后体积会变大
    (include不受static影响,但源文件会受影响)
*/

//向端口写一个byte
static inline void outb(uint16_t port, uint8_t data)
{
    asm volatile ( "outb %b0, %w1" : : "a" (data),
                     "Nd" (port));
}
//向端口写一个word
static inline void outsw(uint16_t port, const void* addr, uint32_t word_cnt)
{
    asm volatile ("cld; rep outsw" : "+S" (addr), 
                    "+c" (word_cnt) : "d" (port));
}
//从端口读取一个byte
static inline uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile ("inb %w1, %b0" : "=a" (data)
                 : "Nd" (port));
    return data;
}
//从端口读取总计word_cnt个word并写入指定的addr
static inline void insw(uint16_t port, void* addr, uint32_t word_cnt)
{
    asm volatile ("cld; rep insw" : "+D" (addr), "+c" (word_cnt)
                : "d" (port) : "memory");
}

#endif