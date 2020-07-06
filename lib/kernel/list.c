#include "./list.h"

//note:结构体当作参数传递最好用指针，除非你就是想用形参传递


/*list_init
@funciton:
    list链表初始化
*/
void list_init(struct list* list)
{
    list->head.prev = NULL;
    list->head.next = &list->tail;

    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

/*list_insert
@function:
    在链表中嵌入一个元素
@param:
    rear:已经存在于链表中的元素；该函数要将elem嵌入到rear前面
    elem:需要被被嵌入的元素
*/
void list_insert(struct list_element* rear, struct list_element* elem) 
{
    intr_status status = intr_get_status();
    rie_intr_disable();
    rear->prev->next = elem;
    elem->prev = rear->prev;
    elem->next = rear;
    rear->prev = elem;
    rie_intr_set(status);
}

/*list_push
@function:
    类似push操作，将elem元素嵌入到第一个位置（head指向它）
*/
void list_push(struct list* list, struct list_element* elem)
{
    list_insert(list->head.next, elem);
}

/*
@function:
    类似pop操作，将head指向的第一个元素弹出；更新head指向被pop元素的后一个元素
*/
struct list_element* list_pop(struct list* list)
{
    //判断list非空
    if(list_empty(list)) {
        rie_puts("list empty,pop failed\r\n");
        ASSERT(list_empty(list) == 0);
    }

    struct list_element* elem_copy = list->head.next;
    list_remove(list->head.next);
    // list->head.next = list->head.next->next;
    // list->head.next->prev = &(list->head);

    return elem_copy;
}


void list_append(struct list* list, struct list_element* elem)
{
    list_insert(&(list->tail),elem);
}


void list_remove(struct list_element* elem)
{
    intr_status status = intr_get_status();
    rie_intr_disable();
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    rie_intr_set(status);
}


/*elem_search
@function:
    搜索指定元素
@return:
    成功找到返回1(true)；否则为0(false)
*/
uint8_t elem_search(struct list* list, struct list_element* obj_elem) 
{
    struct list_element* elem = list->head.next;
    while(elem != (struct list_element*)(&list->tail)){
        if(elem == obj_elem) {return 1;}
        elem = elem->next;
    }
    return 0;
}


/*list_length
@function:
    计算list的长度（不包含头尾的元素个数）
*/
uint32_t list_length(struct list* list)
{
    uint32_t cnt = 0;
    struct list_element* elem = list->head.next;
    while(elem != &(list->tail)){
        cnt++;
        elem = elem->next;
    }
    return cnt;
}


/*list_pickup
@function:
    该函数比较特殊，通过遍历链表寻找第一个符合自定义函数func条件的元素

*/
struct list_element* list_pickup(struct list* list, function func, int argv) 
{
    struct list_element* elem = list->head.next;

    if (list_empty(list)) {return NULL;}

    while (elem != (struct list_element*)(&list->tail)){
        if (func(elem, argv)) {return elem;}
        elem = elem->next;
    }
    return NULL; 
}

/*
@function:
    判断链表是否为空
*/
uint8_t list_empty(struct list* list) 
{
    return (list->head.next == &list->tail ? 1 : 0);
}