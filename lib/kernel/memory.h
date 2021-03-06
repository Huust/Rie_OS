#ifndef _MEMORY_H_
#define _MEMORY_H_





#include "./bitmap.h"
#include "./list.h"

/*
虚拟内存池
pool_bitmap:位图结构体;该位图是对memory_pool的映射
pool_start:虚拟内存池地址
*/
struct virtual_pool{
    bitmap pool_bitmap;
    uint32_t pool_start;   //存放起始地址,非指针
};


/*
物理内存池
pool_size:内存池大小
tips:虚拟内存池默认大小为4GB;而物理内存池只有
一个,其中分kernel/user;它的大小由实际情况定
*/
struct physical_pool{
    bitmap pool_bitmap;
    uint32_t pool_start;      //存放起始地址,非指针 
    uint32_t pool_size;
};

struct mem_block_desc_t {
    uint32_t block_size;    //该类型下每个内存块的大小
    uint32_t block_per_arena;   //描述符下每个arena的block数量（固定）
    struct list free_list;
};

#define desc_type_num   7

#include "./sync.h"
#include "./process.h"
#include "./print.h"
#include "../stdint.h"
#include "../string.h"
#include "./debug.h"
#include "./thread.h"

#define PAGE_SIZE 4096      //4kb = 4096b

#define PG_P_1  1   // 页表项或页目录项存在属性位
#define PG_P_0  0
#define PG_RW_R 0   // R/W 属性位值,读/执行
#define PG_RW_W 2   // R/W 属性位值,读/写/执行
#define PG_US_S 0   // U/S 属性位值,系统级
#define PG_US_U 4   // U/S 属性位值,用户级


/*
内存申请对象memory applicant

*/
enum mem_apply{
    APP_KERNEL = 0, //申请对象为kernel时==0
    APP_USER = 1,   //申请对象为user时==1
};

struct mem_block {
    struct list_element free_elem;
};

void mem_block_desc_init(struct mem_block_desc_t* pdesc);
void mem_struct_init(uint32_t all_mem,uint32_t page_num);
void* get_kernel_page(uint32_t page_num);
uint32_t addr_v2p(uint32_t vaddr);
void* get_a_page(enum mem_apply applicant, uint32_t vaddr);
#endif
