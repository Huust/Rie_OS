#include "memory.h"
#define PAGE_SIZE 4096      //4kb = 4096b
#define KERNEL_HEAP_START 0xc0100000    //为什么这个值看chapter8.md
#define BITMAP_BASE_ADDR 0xc009a000     //为什么这个值理由同上


/*
conception:
    优先初始化物理内存池,因为是固定的.
*/

struct physical_pool kernel_pool;
struct physical_pool user_pool;

struct virtual_pool kernel_heap;

/*physical_pool_init
@function:
    为kernel user的physical_pool结构体做初始化
    方便后续对物理内存的控制管理
@param:
    all_mem:事先利用bios中断获取当前模拟器下可用的全部地址
    page_num:在loader.asm中已经创建好的page数量；这部分算作已被使用的内存
*/
static void physical_pool_init(uint32_t all_mem,uint32_t page_num)
{
    rie_puts("physical_pool_init start\r\n");
    uint32_t page_table_size = page_num * PAGE_SIZE;
    uint32_t used_mem = page_table_size + 0x100000;  //0x100000:低端1MB
    uint32_t free_mem = all_mem - used_mem;
    

    //以页为单位的内存分配，不满一页直接就舍弃；因此整除即可
    uint32_t used_mem_page = used_mem/PAGE_SIZE;
    uint32_t free_mem_page = free_mem/PAGE_SIZE;
    uint32_t kernel_mem_page = free_mem_page/2;
    uint32_t user_mem_page = free_mem_page - kernel_mem_page;   //user kernel对半分内存


    //tag:直接除8会出现:多出来的小于8个页被bitmap忽视
    kernel_pool.pool_bitmap.bitmap_len = kernel_mem_page/8;
    user_pool.pool_bitmap.bitmap_len = user_mem_page/8;


    //bitmap数组存放的位置
    kernel_pool.pool_bitmap.bitmap_set = (uint8_t*)BITMAP_BASE_ADDR;
    user_pool.pool_bitmap.bitmap_set = (uint8_t*)(BITMAP_BASE_ADDR + \
    kernel_pool.pool_bitmap.bitmap_len);    //user bitmap紧跟在kernel bitmap后面


    //physical_pool结构体中的pool_start
    kernel_pool.pool_start = used_mem;
    user_pool.pool_start = used_mem + kernel_mem_page * PAGE_SIZE;

    //physical_pool结构体中的pool_size
    kernel_pool.pool_size = kernel_mem_page * PAGE_SIZE;
    user_pool.pool_size = user_mem_page * PAGE_SIZE;

    //bitmap 初始化
    bitmap_init(kernel_pool.pool_bitmap);
    bitmap_init(user_pool.pool_bitmap);

    //
    rie_puts("kernel_pool bitmap_set:");
    rie_puti((uint32_t)kernel_pool.pool_bitmap.bitmap_set);

    rie_puts("    pool_start:");
    rie_puti(kernel_pool.pool_start);

    rie_puts("\r\n");

    rie_puts("user_pool bitmap_set:");
    rie_puti((uint32_t)user_pool.pool_bitmap.bitmap_set);

    rie_puts("    pool_start:");
    rie_puti(user_pool.pool_start);
    rie_puts("\r\n");

}


/*kernel_virtual_pool_init
@function:
    初始化kernel_heap(内核空闲虚拟内存)
*/
static void kernel_virtual_pool_init(void)
{
    /*note:有趣之处在于,明明内存只有32MB,而且bitmap实际管理
    的是free_mem(<32MB),但我偏偏当成512MB内存划分起始地
    址;不过由于数组length是按照free_mem计算的,所以不会出现
    内存不存在的情况*/

    rie_puts("kernel_heap_init start\r\n");

    //bitmap数组存放的位置(末尾挨着物理内存池bitmap起始处)
    kernel_heap.pool_bitmap.bitmap_set = (uint8_t*)(BITMAP_BASE_ADDR + \
    kernel_pool.pool_bitmap.bitmap_len + user_pool.pool_bitmap.bitmap_len);

    //bitmap数组长度
    /*内核堆kernel_heap的bitmap长度与内核物理内存池的相同
    note:值得注意的是，之所以叫heap而不称之为虚拟内存池是因为
    这个heap管理的是未使用的内核虚拟内存；因为有一部分内存已经被
    内核使用了，所以heap更适合这个名字；否则位图就不是简单地
    全部初始化为0了*/
    kernel_heap.pool_bitmap.bitmap_len = kernel_pool.pool_bitmap.bitmap_len;

    //virtual_pool结构体的内存池地址
    kernel_heap.pool_start = KERNEL_HEAP_START;   
}

/*mem_struct_init
@function:
    内存各种结构体初始化
@param:
    all_mem:全部内存
    page_num:loader.asm中已分配好的page数量
@notes:
    all_mem == 0x2000000
    因为本机内存为32MB(0x2000000)
    page_num == 256
    为什么是256？见问题记录.md 6月7日版
*/
void mem_struct_init(uint32_t all_mem,uint32_t page_num)
{
    //rie_puts("mem_struct_init start\r\n");
    physical_pool_init(all_mem, page_num);
    kernel_virtual_pool_init();
    //rie_puts("mem_struct_init done\r\n");
}

/*-----------------分隔符-------------------*/


/*vmalloc
@function:
    虚拟内存分配 virtual memory allocation
    在虚拟内存池中分配数量为n的页
@param:
    applicant:枚举型，判断申请对象是kernel/user
    page_num:需要分配的页数量(这些页连续)
@return:
    返回值：在虚拟内存池中分配得到的连续页的起始地址
    return 0:正常不返回0，此时代表分配异常
*/
static uint32_t vmalloc(enum mem_apply applicant, uint32_t page_num)
{
    int32_t bitmap_idx = 0;

    //首先内核的虚拟内存池bitmap查看是否存在连续也可用
    if(applicant == APP_KERNEL){
        bitmap_idx = bitmap_scan(kernel_heap.pool_bitmap, page_num);
    }else{
        //note:内存申请对象是user，暂时不做考虑
    }
    
    if(bitmap_idx == -1){return 0;}

    //找到可用页，bitmap置位
    for(int32_t i = bitmap_idx;i < bitmap_idx+page_num;i++)
    {bitmap_setval(kernel_heap.pool_bitmap,i,1);}

    //返回值是分配到的连续页的起始地址
    return (KERNEL_HEAP_START + bitmap_idx * PAGE_SIZE);
}


/*get_vaddr_pte
@function:
    获取已知虚拟地址的pte地址(也是用虚拟地址表示)
@param:
    vaddr：虚拟地址
@return:
    pte地址(虚拟地址，方便之后直接读写那部分内容)
*/
static uint32_t get_vaddr_pte(uint32_t vaddr)
{
    return ((0xffc00000)+((vaddr&0xffc00000)>>10)+4*((vaddr&0x3ff000)>>12));
}

/*get_vaddr_pde
@function:
    获取已知虚拟地址的pde地址(也是用虚拟地址表示)
@param:
    vaddr：虚拟地址
@return:
    pde地址(虚拟地址，方便之后直接读写那部分内容)
@conception:
    见chapter9.md 三次欺骗cpu
    pde部分要做两次索引第1024个pde
*/
static uint32_t get_vaddr_pde(uint32_t vaddr)
{
    return ((0xffc00000)+(0xffc00000>>10)+4*((vaddr&0xffc00000)>>22));
}


/*
@function:
    物理页的分配(每次调用只能分配1页)
@param:
    物理内存池结构体
@return:
    返回被分配页的起始物理地址(!!!注意是物理地址)
    0:分配失败
*/
static uint32_t pmalloc(struct physical_pool pool)
{
    int32_t bitmap_idx = bitmap_scan(pool.pool_bitmap, 1);
    if(bitmap_idx == -1){return 0;}     //return 0是因为正常返回不会为0
    else{
        bitmap_setval(pool.pool_bitmap,bitmap_idx,1);
        return (pool.pool_start + bitmap_idx*PAGE_SIZE);
    }
}


/*mem_map
@function:
    建立vaddr与实际物理内存的映射
@param:
    vaddr:虚拟地址
    paddr:页的物理地址
@notes:
    因为物理页是非连续的，所以每次只能映射1页
*/
static void mem_map(uint32_t vaddr,uint32_t paddr)
{
    //通过vaddr获得pde、pte的虚拟地址
    uint32_t* vaddr_pde = (uint32_t*)get_vaddr_pde(vaddr);
    uint32_t* vaddr_pte = (uint32_t*)get_vaddr_pte(vaddr);

    uint32_t paddr_pt = 0;  //存放通过pmalloc()新建的page table的物理地址

    //首先看pde
    if((*vaddr_pde)&PG_P_1){    //如果pde存在
        //那么应该接着判断pte是否存在
        if((*vaddr_pte)&PG_P_1){    //pte也存在
            //为pte更新物理地址
            //fixme:然而这种情况不允许你写;此时映射已经建立好了.改不改都没有意义
            *vaddr_pte = (*vaddr_pte)&0xfff + paddr;

        }else{                    /*pte不存在*/
            //创建pte
            paddr_pt = pmalloc(kernel_pool);
            *vaddr_pde = (*vaddr_pde)&0xfff + paddr_pt;
            //对新生成的page table全部清0，防止这是之前被释放的内存，内部存在dirty data
            //rie_memset((void*)((uint32_t)vaddr_pte&0xfffff000),0,PAGE_SIZE);
            rie_memset(vaddr_pte,0,4);
            //属性位
            *vaddr_pte = (paddr | PG_US_U | PG_RW_W | PG_P_1);
            rie_puts("\r\nvaddr_pte:");
            rie_puti(*vaddr_pte);
        }
    }else{                      /*如果pde不存在(自然pte也不存在)*/
        paddr_pt = pmalloc(kernel_pool);
        //为pde赋值(在loader.asm中已经为1024pde预留了空间，但是当时pde下面没有页表就没有为其初始化)
        rie_memset(vaddr_pde,0,4);
        *vaddr_pde = (paddr_pt | PG_US_U | PG_RW_W | PG_P_1);
        //为新创建的pte赋值
        rie_memset(vaddr_pte,0,4);
        *vaddr_pte = (paddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}


/*page_malloc
@function:
    整合了按页分配需要的函数：虚拟地址分配、物理地址分配、页表映射
@param:
    mem_enum:内存申请对象
    page_num:申请的页数
@return:
    page_vaddr:分配后的连续页的起始虚拟地址
    0:page_vaddr分配错误
    1:page_paddr分配错误
*/
static uint32_t page_malloc(enum mem_apply applicant, uint32_t page_num)
{
    uint32_t cnt = page_num;
    uint32_t offset = 0;    /*因为多个页分配时，虚拟内存是连续的
                            而物理页每次只能分配一个，所以需要
                            变量offset控制映射时所需要的地址*/
    
    //虚拟内存池中页的地址
    uint32_t page_vaddr = vmalloc(applicant,page_num);
    if(page_vaddr == 0) {return 0;}

    uint32_t page_paddr = 0;    

    struct physical_pool pool;
    pool = (applicant == APP_KERNEL)?kernel_pool:user_pool;
    
    if(applicant == APP_KERNEL){
        while(cnt--){
            page_paddr = pmalloc(pool);
            if(page_paddr == 0) {
                /*因为物理页每次分配一页，所以可能在中途就没有空闲页了
                此时需要将之前已做好映射的内存全部回滚*/
                //todo:在之后的内存回收部分将完成虚拟页、物理页的回收
                return 1;
            }
            mem_map(page_vaddr + offset,page_paddr);
            offset += PAGE_SIZE;
        }
    }
    return page_vaddr;
}


/*get_kernel_page
@function:
    申请page_num页的内核内存
@param:
    page_num:申请的页数
@return:
    虚拟页起始地址
*/
void* get_kernel_page(uint32_t page_num)
{

    uint32_t page_vaddr = page_malloc(APP_KERNEL,page_num);
    if((page_vaddr == 0)||(page_vaddr == 1)) {return NULL;}
    rie_memset((void*)page_vaddr,0,page_num * PAGE_SIZE);
    return (void*)page_vaddr;
}

