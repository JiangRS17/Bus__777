#include "softbus_new.h"
#include "softbus_error.h"
#include<stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include<pthread.h>
#include<sys/time.h>
// #include "cmsis_os2.h"

#define MAX_MALLOC_SIZE (512*1024*1024) //512M
#define MS_PER_SECOND 1000

void *SoftBusMalloc(unsigned int size)
{
    if (size > MAX_MALLOC_SIZE){
        return NULL;
    }

    return malloc(size);
}

void SoftBusFree(void *pt)
{
    if (pt == NULL) {
        return;
    }

    free(pt);
}


// 创建软总线列表
SoftBusQueues *CreateSoftBusList()
{
    if (busCounter >= SOFTBUS_MAX_LISTS)
    {
        printf("Error: Cannot create more soft buses. Maximum limit reached.\n");
        return NULL;
    }

    // 分配内存给新的软总线
    SoftBusQueues *newSoftBus = (SoftBusQueues *)mallc(sizeof(SoftBusQueues));
    if (newSoftBus == NULL)
    {
        printf("Error: Memory allocation failed for new soft bus.\n");
        return NULL;
    }

    // 初始化软总线名称 (这里假设每个软总线有一个简单的编号)
    newSoftBus->Name = 'A' + busCounter;

    // 初始化每个队列
    for (int i = 0; i < SOFTBUS_MAX_QUEUES; i++)
    {
        SoftBusList *queue = &newSoftBus->queue[i];
        queue->cnt = 0;
        pthread_mutex_init(&queue->lock,NULL); // 初始化每个队列的锁
        ListInit(&queue->list);

        if (i==0){
            snprintf(queue->name, NAME_LENGTH, "glistpendingconnect");
        } else if (i==1) {
            snprintf(queue->name, NAME_LENGTH, "glistalreadyconnect");
        } else if (i == 2) {
            snprintf(queue->name, NAME_LENGTH, "glistpending");
        }
        
    }
    
    // 将新创建的软总线加入到 globalSoftBusList 数组中
    globalSoftBusList[busCounter] = newSoftBus;

    busCounter++;

    return newSoftBus;
}


int32_t DestroySoftBusList(int busIndex)
{
    if (busIndex < 0 || busIndex >= busCounter) {
        printf("Error: Invalid bus index.\n");
        return SOFTBUS_ERR; 
    }

    // 获取待销毁的软总线
    SoftBusQueues *softbusToDestroy = globalSoftBusList[busIndex];

    // 销毁软总线中的每个队列
    for (int i = 0; i < SOFTBUS_MAX_QUEUES; i++)
    {
        SoftBusList *queue = &softbusToDestroy->queue[i];

        // 销毁队列的锁
        pthread_mutex_destroy(&queue->lock);

        // 清理队列中的链表
        ListDelete(&queue->list);

    }

    globalSoftBusList[busIndex] = NULL;

    // 释放软总线内存
    free(softbusToDestroy);

    busCounter--;
    
    return SOFTBUS_OK;
}


int32_t DestroyAllSoftButList()
{
    // 遍历所有软总线并销毁
    for (int i = 0; i < busCounter; i++)
    {
        if (globalSoftBusList[i] != NULL)
        {
            DestroySoftBusList(i);
        }
    }

    busCounter = 0;

    return SOFTBUS_OK;
    
}


//创建一条新的softbus
//以链表的数据结构来描述softbus
// SoftBusList *CreateSoftBusList()
// {
//     SoftBusList *list = (SoftBusList *)SoftBusMalloc(sizeof(SoftBusList));
//     if (list == NULL)
//     {
//         printf("Failed to allocate memory for SoftBusList\n");
//         return SOFTBUS_ERR;
//     }
//     //名为list的内存区域，大小为sfotBusList结构体的大小全部清零
//     (void)memset_s(list,sizeof(SoftBusList),0,sizeof(SoftBusList));

//     //need add Mutex
//     ListInit(&list->list); 

//     list->cnt = 1; 
//     list->busId = busCounter;
//     //控制器初始化s
//     //适配器初始化

//     // 将新创建的软总线添加到全局数组
//     for (int i = 0; i < SOFTBUS_MAX_LISTS; i++)
//     {
//         if(globalSoftBusList[i] == NULL) {
//             globalSoftBusList[i] = list;
//             break;
//         }
//     }
    
//     return list;
// }

//destroy softbus
// int32_t DestroySoftBusList(unsigned int busId)
// {
//     for (int i = 0; i < SOFTBUS_MAX_LISTS; i++) {
//         if (globalSoftBusList[i] != NULL && globalSoftBusList[i]->busId == busId) {
//             SoftBusFree(globalSoftBusList[i]);  // 释放内存
//             globalSoftBusList[i] = NULL; // 清空全局数组中对应的位置
//             return SOFTBUS_OK;  // 销毁成功
//         }
//     }
//     printf("SoftBusList with busId %u not found.\n", busId);
//     return SOFTBUS_ERR;  // 没有找到对应的软总线
    
//     // ListDelInit(&list->list);
//     // SoftBusMutexDestory(&list->lock);
//     // SoftBusFree(list);
//     return SOFTBUS_OK;
// }



// int32_t SoftBusCondDestroy(SoftBusCond *cond)
// {
//     if ((cond == NULL) || ((void *)(*cond) == NULL))
//     {
//         printf("cond is null");
//         return SOFTBUS_INVALID_PARAM;
//     }
    
//     int ret;
//     ret = pthread_cond_destory((pthread_cond_t *)*cond);
//     if (ret != 0)
//     {
//         SoftBusFree((void *)*cond);
//         *cond = (SoftBusCond)NULL;
//         return SOFTBUS_ERR;
//     }
//     SoftBusFree((void *)*cond);
//     *cond = (SoftBusCond)NULL;
//     return SOFTBUS_OK;
// }


//有问题没写完
// int32_t SoftBusCondInit(SoftBusCond *cond)
// {
//     if (cond == NULL)
//     {
//         printf("cond is null");
//         return SOFTBUS_INVALID_PARAM;
//     }

//     pthread_condattr_t attr = {0};
//     int ret = pthread_cond_init(&attr);

//     if(ret != 0) {
//         return SOFTBUS_ERR;
//     }
//     // ret = pthread_condattr_setlock(&attr,CLOCK_MONOTONIC);
    
// }

// void *SoftBusCreateTimer(void **timerId, void *timerFunc, unsigned int type)
// {
//     (void)timerId;

//     void *id = osTimerNew((osTimerFunc_t)timerFunc,type,NULL,NULL);

//     if(id != NULL){
//         printf("create timer success");
//         return id;
//     }
//     printf("create time failed");
//     return NULL;
// }

// int SoftBusStartTimer(void *timerId, unsigned int ms)
// {
//     if(osTimerStrat(timerId,ms * osKernelGetTickFreq()/MS_PER_SECOND) != osOK) {
//         printf("strat timer failed");
//         return SOFTBUS_ERR;
//     }

//     printf("start timer success");
//     return SOFTBUS_OK;
// }

// int SoftBusDeleteTimer(void *timerId)
// {
//     if(osTimerDelete(timerId) != osOK) {
//         printf("delete timer failed");
//         return SOFTBUS_ERR;

//     }

//     printf("delete timer success");
//     return SOFTBUS_OK;
// }


// int32_t SoftBusGetTime(SoftBusSysTime *sysTime) 
// {
//     if(sysTime == NULL) {
//         printf("system time is null");
//         return SOFTBUS_INVALID_PARAM;
//     }
//     struct timeval time = {0};
//     gettimeofday(&time, NULL);

//     sysTime->sec = time.tv_sec;
//     sysTime->usec = time.tv_usec;

//     return SOFTBUS_OK;
// }
