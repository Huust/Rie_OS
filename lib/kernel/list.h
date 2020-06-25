#ifndef _LIST_H_
#define _LIST_H_

#include "../stdint.h"
#include "./interrupt.h"
#include "./print.h"
#include "./debug.h"


/*list_element结构体
@function:
    链表元素，因为没有data所以只有prev next两个
    指向该元素前后相邻两元素的指针
@member:
*/
struct list_element{
    struct list_element* prev;
    struct list_element* next;
};

/*list结构体
@function:
    存放一个完整双向链表的头尾元素
@member:
    head
    tail
*/
struct list{
    struct list_element head;
    struct list_element tail;
};

/*自定义函数类型，成功返回1，否则返回0*/
typedef uint8_t (function)(struct list_element*, 
                        int);

void list_init(struct list* list);
void list_insert(struct list_element* rear,
                 struct list_element* elem);
void list_push(struct list* list,
                 struct list_element* elem);
struct list_element* list_pop(struct list* list);
                
void list_append(struct list* list, \
                 struct list_element* elem);

void list_remove(struct list_element* elem);


uint8_t elem_search(struct list* list, struct list_element* obj_elem); 

uint32_t list_length(struct list* list);

struct list_element* list_pickup(struct list* list,    \
                         function func, int argv);

uint8_t list_empty(struct list* list); 

#endif