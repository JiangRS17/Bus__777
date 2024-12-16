#ifndef SOFT_ADBITER_H
#define SOFT_ADBITER_H

#include "soft_control.h"
#include "../../adapter/include/soft_adapter.h"

typedef struct 
{
    pthread_mutex_t arbiterlock; 
    bool isBusOccupied;           // 是否有设备正在使用总线
    SoftBusQueues *associatedBus;  // 软总线
    int currentBus;
} BusArbiter;


BusArbiter *globalBusArbiter[SOFTBUS_MAX_LISTS] = {NULL};


// 接口函数：选择 FIFO 调度策略
int32_t ArbiterAllocateBus_FIFO(PublishInfo *publishInfo); 

// 接口函数：选择优先级调度策略
int32_t ArbiterAllocateBus_Priority(PublishInfo *publishInfo);

BusArbiter *CreateBusArbiter(SoftBusQueues *bus);

int32_t InitializeBusArbiters();

int SoftBusArbiter_RequestBus(BusArbiter *arbiter, PublishInfo *publishInfo);

int32_t HandlerClockInterrupt(int currentBus);

int32_t SoftArbiterInit();



#endif