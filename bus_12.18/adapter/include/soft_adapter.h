#ifndef SOFT_ADAPTER_H
#define SOFT_ADAPTER_H

#include "../../controller/include/soft_control.h"
#include "../../../bridge/manager/include/bridge_manager.h"
#include "../../../bridge/transmission/include/trans_manager.h"


#include <stdlib.h>

extern int globalAdapterCount;

typedef struct 
{
    int adapterId;                        // 适配器唯一标识
    SoftBusQueues *associatedBus[SOFTBUS_MAX_LISTS];         // 适配的软总线
    bool isAvailable;                     // 当前适配器是否可用
    pthread_mutex_t adapterLock;          // 适配器锁
    BridgeInfo *bridges[SOFTBUS_MAX_LISTS];   // 管理的桥节点
} SoftBusAdapter;

extern SoftBusAdapter *globalAdapter[SOFTBUS_MAX_LISTS];


extern SoftBusAdapter *CreateSoftBusAdapter(int adapterId);

extern int AdapterSendRequest(SoftBusAdapter *adapter, PublishInfo *publishInfo, BridgeInfo *bridge);

extern int RouteRequest(int adapterId, PublishInfo *publishInfo);

// int RouteToComponent(int adapterId, PublishInfo *publishInfo);

extern void ProcessComponentRequest(BridgeInfo *bridge, PublishInfo *publishInfo);

extern int ProcessPublishInfoRequest(int adapterId);

extern int32_t DestroySoftBusAdapter(int adapterId);

extern int32_t DestroyAllSoftBusAdapter();


extern int32_t Soft_Adapter_Init();

extern int32_t Soft_Adapter_DeInit(int adpterId); 

extern void Soft_Adapter_AllDeInit();

#endif


//·从一个状态变化到另一个状态，需要队列删除
//对于不同组件，提供统一发送和接受消息方式