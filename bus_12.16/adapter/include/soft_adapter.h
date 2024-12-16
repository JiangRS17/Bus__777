#ifndef SOFT_ADAPTER_H
#define SOFT_ADAPTER_H

#include "../../controller/include/soft_control.h"
#include "../../../bridge/manager/include/bridge_manager.h"
#include "../../bus/include/softbus_list.h"
#include "../../../bridge/transmission/include/trans_manager.h"


#include <stdlib.h>

int globalAdapterCount = 0;

typedef struct 
{
    int adapterId;                        // 适配器唯一标识
    SoftBusQueues *associatedBus[SOFTBUS_MAX_LISTS];         // 适配的软总线
    bool isAvailable;                     // 当前适配器是否可用
    pthread_mutex_t adapterLock;          // 适配器锁
    BridgeInfo *bridges[SOFTBUS_MAX_LISTS];   // 管理的桥节点
} SoftBusAdapter;

SoftBusAdapter *globalAdapter[SOFTBUS_MAX_LISTS] = {NULL};


SoftBusAdapter *CreateSoftBusAdapter(int adapterId);

int AdapterSendRequest(SoftBusAdapter *adapter, PublishInfo *publishInfo, BridgeInfo *bridge);

int RouteRequest(int adapterId, PublishInfo *publishInfo);

// int RouteToComponent(int adapterId, PublishInfo *publishInfo);

void ProcessComponentRequest(BridgeInfo *bridge, PublishInfo *publishInfo);

int ProcessPublishInfoRequest(int adapterId);

int32_t DestroySoftBusAdapter(int adapterId);

int32_t DestroyAllSoftBusAdapter();


int32_t Soft_Adapter_Init();

int32_t Soft_Adapter_DeInit(int adpterId); 

void Soft_Adapter_AllDeInit();

#endif


//·从一个状态变化到另一个状态，需要队列删除
//对于不同组件，提供统一发送和接受消息方式