#ifndef SOFTBUS_LIST_H
#define SOFTBUS_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *data; 
} ListNode;


//用于初始化双向循环链表的函数

__attribute__((always_inline)) static inline void ListInit(ListNode *list)
{
    list->next = list;
    list->prev = list;
    list->data = NULL;
}

//get list head node 
#define GET_LIST_HEAD(object) ((object)->next)

//get list tail node 
#define GET_LIST_TAIL(object) ((object)->prev)

//insert a new node to list
__attribute__((always_inline)) static inline void ListAdd(ListNode *list, ListNode *node)
{
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}

//delete a node from list
__attribute__((always_inline)) static inline void ListDelete(ListNode *node)
{
    if(node->next != 0 && node->prev !=0 ){
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    node->next = node;
    node->prev = node;
}

//List empty
__attribute__((always_inline)) static inline bool IsListEmpty(const ListNode *node)
{
    return (bool)(node->next == node);
}

__attribute__((always_inline)) static inline void ListDel (ListNode *prevNode, ListNode * nextNode)
{
    nextNode->prev = prevNode;
    prevNode->next = nextNode;
}


//delete node and initialize list
__attribute__((always_inline)) static inline void ListDelInit(ListNode *list)
{
    ListDel(list->prev,list->next);
    ListInit(list);
}


#endif