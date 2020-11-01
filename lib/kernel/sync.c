#include "./sync.h"

/* semaphore初始化   sem_value:信号量的值（1代表互斥信号量）*/
void sem_init(struct semaphore* sem, uint8_t sem_value)
{
    sem->value = sem_value;
    list_init(&sem->wait_list);
}

void lock_init(struct lock* plock)
{
    plock->owner = NULL;
    plock->repeat_num = 0;
    sem_init(&plock->sem, 1);
}

/*
note:需要强调,sem_down必然是线程自己去做这件事
    若sem!=0,则正常获得信号量;
    若sem==0,则此线程自己阻塞自己
*/
void sem_down(struct semaphore* sem)
{
    intr_status old_status = intr_get_status();
    rie_intr_disable();

    // struct thread_pcb* current_thread = get_running_thread();

    while (sem->value == 0) {
        struct thread_pcb* current_thread = get_running_thread();

        //判断wait_list中是否已有该线程的elem
        ASSERT(!elem_search(&sem->wait_list, 
                            &current_thread->ready_list_elem));

        list_append(&sem->wait_list, &current_thread->ready_list_elem);
        thread_block(TASK_BLOCK);
    }
    sem->value --;
    ASSERT(sem->value == 0);

    rie_intr_set(old_status);
}

void sem_up(struct semaphore* sem)
{
    intr_status old_status = intr_get_status();
    rie_intr_disable();

    ASSERT(sem->value == 0);    /* 只考虑最基本的互斥信号量 */
    if (!list_empty(&sem->wait_list)) {
        struct thread_pcb* thread_blocked = elem2pcb(struct thread_pcb, \
                                                list_pop(&sem->wait_list), \
                                    offset(struct thread_pcb, ready_list_elem));        
        thread_unblock(thread_blocked);
    }
    sem->value ++;

    ASSERT(sem->value == 1);

    rie_intr_set(old_status);
}

// note:注意,这才是线程真正需要调用的函数
void lock_acquire(struct lock* plock)
{
    struct thread_pcb* cur_thread = get_running_thread();
    if (plock->owner != cur_thread) {
        sem_down(&plock->sem);
        ASSERT(plock->repeat_num == 0);
        plock->repeat_num = 1;
        plock->owner = cur_thread;
    } else {
        plock->repeat_num ++ ;
    }
}

void lock_release(struct lock* plock)
{
    //确保只有锁的拥有者才能释放锁
    ASSERT(plock->owner == get_running_thread());
    if (plock->repeat_num > 1) {
        plock->repeat_num -- ;
        return;
    } 

    ASSERT(plock->repeat_num == 1);
    plock->owner = NULL;
    plock->repeat_num = 0;
    sem_up(&plock->sem);
}