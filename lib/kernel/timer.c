#include "./timer.h"


#define IRQ0_FREQ 100
#define CLK_FREQ 1193180
#define COUNTER0_VAL CLK_FREQ / IRQ0_FREQ
#define COUNTER0_PORT 0x40
#define COUNTER0_NUM 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CTRL_PORT 0x43


static void freq_set(uint8_t counter_port,
                    uint8_t counter_num,
                    uint8_t rwl,
                    uint8_t counter_mode,
                    uint16_t counter_val)
{
    outb(PIT_CTRL_PORT,
    (uint8_t)(counter_num << 6 | rwl << 4 | counter_mode << 1));
    outb(counter_port, (uint8_t)counter_val);
    outb(counter_port, (uint8_t)counter_val >> 8);
}


//interrupt gate下中断标识符IF在进入中断处理函数时已经自动置为0
void intr_timer_handler(uint8_t intr_number)
{
    ASSERT(intr_close == get_status());
    struct thread_pcb* cur_thread = get_running_thread();
    ASSERT(cur_thread->bound_detect == 0x20000803);
    cur_thread->all_tick ++;
    if(cur_thread->tick == 0) {schedule();}
    else {cur_thread->tick --;}
}

/*
必须在中断初始化idt_init()之后，否则IRQ0注册会被覆盖
*/
void timer_init(void)
{
    freq_set(COUNTER0_PORT, 
            COUNTER0_NUM,
            READ_WRITE_LATCH,
            COUNTER_MODE,
            COUNTER0_VAL);
    intr_handler_register(0, intr_timer_handler);
    rie_puts("timer init done");
}



