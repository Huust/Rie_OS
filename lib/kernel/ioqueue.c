#include "./ioqueue.h"

void ioqueue_init(struct ioqueue* ioq)
{
    lock_init(&ioq->lock);
    rie_memset(ioq->buffer, 0, 64);
    ioq->head_idx,ioq->tail_idx = 0;
    ioq->producer,ioq->consumer = NULL;
}


int32_t get_next_pos(int32_t cur_pos, uint8_t len)
{
    return (cur_pos + 1)%len;
}


/*note:
    判断是否为空,这里我们认为当只剩下一个byte的资源可用时,即为empty
    也就是说,在buffer满时,consumer消耗到63个字节时,便会通知producer
    为空,似乎buffer只有63字节*/
uint8_t ioq_empty(struct ioqueue* ioq)
{
    if (get_next_pos(ioq->tail_idx, 64) == ioq->head_idx) {return 1;}
    else {return 0;}
}


uint8_t ioq_full(struct ioqueue* ioq)
{
    if (ioq->head_idx == ioq->tail_idx) {return 1;}
    else {return 0;}
}


//fixme:作者为什么要用二级指针做为参数传入(那个ASSERT感觉意义不大)
//查看实际使用该函数
void ioq_wait(struct thread_pcb* pthread)
{
    //ASSERT();
    pthread = get_running_thread();
    thread_block(TASK_BLOCK);
}


void ioq_wakeup(struct thread_pcb* pthread)
{
    //ASSERT();
    thread_unblock(pthread);
}


int8_t ioq_getchar(struct ioqueue* ioq)
{
    while (ioq_empty(ioq)) {
        lock_acquire(&ioq->lock);
        ioq_wait(ioq->consumer);
        lock_release(&ioq->lock);
    }

    int8_t byte = ioq->buffer[ioq->tail_idx];
    ioq->tail_idx = get_next_pos(ioq->tail_idx, 64);

    if (ioq->producer != NULL) {
        ioq_wakeup(ioq->producer);
    }
    
    return byte;
}


void ioq_putchar(struct ioqueue* ioq, int8_t byte)
{
    while (ioq_full(ioq)) {
        lock_acquire(&ioq->lock);
        ioq_wait(ioq->producer);
        lock_release(&ioq->lock);
    }
    
    ioq->buffer[ioq->head_idx] = byte;
    ioq->head_idx = get_next_pos(ioq->head_idx, 64);

    if (ioq->consumer != NULL) {
        ioq_wakeup(ioq->consumer);
    }
}