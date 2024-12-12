#ifndef SOFT_ADAPTER_H
#define SOFT_ADAPTER_H

#include "soft_control.h"
#include "bridge_manager.h"


SoftBusAdapter *globalAdapter[SOFTBUS_MAX_LISTS] = {NULL};
int globalAdapterCount = 0;

typedef struct 
{
    int adapterId;                        // 适配器唯一标识
    SoftBusQueues *associatedBus[SOFTBUS_MAX_LISTS];         // 适配的软总线
    bool isAvailable;                     // 当前适配器是否可用
    pthread_mutex_t adapterLock;          // 适配器锁
    BridgeInfo *bridges[BRIDEGE_MAX_NUM];   // 管理的桥节点
} SoftBusAdapter;


SoftBusAdapter *CreateSoftBusAdapter(int adapterId);

int AdapterSendRequest(int adapterId, PublishInfo *publishInfo);

int RouteRequest(int adapterId, const char *requestedFunId);

int RouteToComponent(int adapterId, PublishInfo *publishInfo);

void ProcessComponentRequest(BridgeInfo *bridge, PublishInfo *publishInfo);

int ProcessPublishInfoRequest(int adapterId);

int32_t DestroySoftBusAdapter(int adapterId);

int32_t DestroyAllSoftBusAdapter();

#endif


//·从一个状态变化到另一个状态，需要队列删除
//对于不同组件，提供统一发送和接受消息方式