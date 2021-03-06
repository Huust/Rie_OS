#ifndef _BIT_MAP_H_
#define _BIT_MAP_H_
#include "../stdint.h"
#include "../string.h"
#include "./debug.h"
#include "./print.h"
typedef struct bit_map{
    uint8_t* bitmap_set;    //指向存放位图的数组
                            //这里应该是uint8_t而不是uint32_t
    uint32_t bitmap_len;    //数组的长度(byte为单位)
}bitmap; 

#define BITMAP_MASK 1
void bitmap_init(bitmap* bitmap_demo);
uint8_t bitmap_test(bitmap* bitmap_demo,uint32_t bitmap_index);
int32_t bitmap_scan(bitmap* bitmap_demo,uint32_t cnt);
void bitmap_setval(bitmap* bitmap_demo,uint32_t bitmap_index,uint8_t value);
#endif