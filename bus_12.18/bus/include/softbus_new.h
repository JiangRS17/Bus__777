#ifndef SOFTBUS_NEW_H
#define SOFTBUS_NEW_H

#include "softbus_list.h"
#include "../../include/softbus_error.h"
#include <stdint.h>
#include <pthread.h>

#define SOFTBUS_MAX_LISTS 100   // 最大软总线数
#define SOFTBUS_MAX_QUEUES 4  // 每条软总线上有 x 个队列
#define NAME_LENGTH 32             // 队列名称的最大长度



extern int busCounter;     // 当前总线编号

//SoftBus
typedef pthread_mutex_t SoftBusMutex;

typedef struct {
    SoftBusMutex lock;
    unsigned int cnt; 
    ListNode list;
    char name[NAME_LENGTH];    // 队列名称
} SoftBusList;


typedef struct 
{
    char Name;
    SoftBusList queue[SOFTBUS_MAX_QUEUES];
} SoftBusQueues;


SoftBusQueues *globalSoftBusList[SOFTBUS_MAX_LISTS];  // 存储所有软总线的数组

// // 创建组件等待连接队列
// SoftBusList *CreatePendConnList();

// // 创建组件已经连接队列
// SoftBusList *CreateAlreadyConnList();


extern int32_t *SoftBusMalloc(unsigned int size);

extern SoftBusQueues *CreateSoftBusList();

extern int32_t DestroySoftBusList(int busIndex);

extern int32_t DestroyAllSoftButList();

extern int32_t SoftBusMgrInit(void);

extern int32_t SoftBusMgrDeinit(int busIndex);


#endif