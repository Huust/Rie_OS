#include "bitmap.h"
/*  bitmap.c
设计思路:四个函数构成位图操作的基本功能
1.位图结构体初始化
2.检测位图中某一位是否被使用的测试函数
3.已知需要n个连续空闲位,构建函数寻找第一个符合要求的位置,返回数组索引
4.用来对以bit为索引的某一位更新0/1
*/


/*bitmap_init
@function:bitmap初始化;
          在创建好一个bitmap结构体后调用该函数初始化
*/
void bitmap_init(bitmap* bitmap_demo)
{
    rie_memset(bitmap_demo->bitmap_set,0,bitmap_demo->bitmap_len);
}


/*bitmap_test
@function:
    判断某个bit_index是否被占用，被bitmap_scan调用
@param:
    bitmap_index:假设值为8,则它在bitmap[1]这个字节的第0位(8/8=1...0)
@return:
    返回1代表该位为1,即被占用
    返回0:未被占用
*/
uint8_t bitmap_test(bitmap* bitmap_demo,uint32_t bitmap_index)
{
    uint32_t byte_index = bitmap_index/8;
    uint32_t bit_index = bitmap_index%8;    //bit_index:剩下不能凑成一个byte的bit数

    if(bitmap_demo->bitmap_set[byte_index]&(BITMAP_MASK<<bit_index))
    return 1;

    return 0;
}


/*bitmap_scan
@function:
    在bitmap中寻找连续cnt个空闲位，该函数每一位是对内存中每一页的映射
@conception:
    首先按byte索引,如果元素==0xff并且索引已经达到数组length,
    宣布未找到cnt个连续的bit,返回-1;
    
    若!=0xff则至少有一个位置.此时先获取该byte中第一个空闲bit的
    位置,接着判断cnt是否为1,若为1就寻找成功,直接返回0

    若cnt不为1,则需要继续从该位向后判断.(注意可能会跨byte)
    如果连续位数量不满足要求,则继续向后线性暴力索引???
    
@param:
    cnt:要求找到连续cnt个空闲位

@return:
    -1代表没有找到
    !=-1则代表返回的是bitmap中的索引(位为单位)
*/
int32_t bitmap_scan(bitmap* bitmap_demo,uint32_t cnt)
{
    uint32_t pos = 0;
    //fixme:这里pos会出现越界情况
    //某一个元素==0xff代表全为1,都被占用
    while((bitmap_demo->bitmap_set[pos]==0xff)&&(pos<bitmap_demo->bitmap_len)){
        pos++;            
    }
    if(pos==bitmap_demo->bitmap_len) {return -1;}    //跳出while原因1:达到length时仍未找到

    uint32_t first_byte_idx = pos;
    uint8_t first_bit_idx = 0;
    uint32_t bitmap_index = 0;

    //第一次元素!=0xff时,代表有可用位;这时去寻找这个byte中第一次出现0的bit
    while(bitmap_demo->bitmap_set[first_byte_idx]&(BITMAP_MASK<<first_bit_idx)){
        first_bit_idx ++;
    }
    bitmap_index = first_byte_idx*8+first_bit_idx;
    if(cnt==1) {return (int32_t)bitmap_index;}      //cnt要求为1时,则已经找到
    
    uint32_t count = 1;     //count:实时记录已经连续的空闲位(至少已有一个空闲bit)
    while(count<cnt){
        //判断剩下的bit数是否超过cnt
        if((bitmap_index+cnt)>(bitmap_demo->bitmap_len-1)) {return -1;}

        if(bitmap_test(bitmap_demo,bitmap_index+count)){
            count = 1;
            //连续空闲位失败,寻找下一次出现空闲位的bitmap_index值
            while(bitmap_test(bitmap_demo,bitmap_index)){
                bitmap_index++;
            }
        }else{
            count++;
        }
    }
    return ((int32_t)bitmap_index);
}


/*
@param:
    value:设置bitmap_index的值为value(0/1)
*/
void bitmap_setval(bitmap* bitmap_demo,uint32_t bitmap_index,uint8_t value)
{
    ASSERT(value==1 || value == 0);

    uint32_t byte_index = bitmap_index/8;
    uint32_t bit_index = bitmap_index%8;

    //value设为0时需要~按位取反后做与运算
    if(value == 1) {bitmap_demo->bitmap_set[byte_index]|=(BITMAP_MASK<<bit_index);}
    else {bitmap_demo->bitmap_set[byte_index]&=~(BITMAP_MASK<<bit_index);}
}