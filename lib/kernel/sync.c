#include "./sync.h"
extern struct list ready_list;

void sem_init(struct semaphore* sem)
{
    sem->value = 1;
    list_init(&sem->wait_list);
}

void lock_init(struct lock* plock)
{
    plock->owner = NULL;
    plock->repeat_num = 0;
    sem_init(plock->sem);
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

    struct thread_pcb* current_thread = get_running_thread();

    while (sem->value == 0) {
        //判断wait_list中是否已有该线程的elem
        ASSERT(!elem_search(&sem->wait_list, 
                            &current_thread->ready_list_elem));

        //todo:修改ready_list_elem名称,因为它不仅仅存放于ready_list中
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

    ASSERT(sem->value == 0);
    if (!list_empty(&sem->wait_list)) {
        list_push(&ready_list, list_pop(&sem->wait_list));
    }
    sem->value ++;

    rie_intr_set(old_status);
}

//note:注意,这才是线程真正需要调用的函数
void lock_acquire(struct lock* plock)
{
    //todo:作者没有加锁,但我感觉需要加
    if (plock->owner != get_running_thread()) {
        sem_down(&plock->sem);
        ASSERT(plock->repeat_num == 0);
        plock->repeat_num = 1;
        plock->owner = get_running_thread();
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
    } else {
        plock->owner = NULL;
        plock->repeat_num = 0;
        sem_up(&plock->sem);
    }
}