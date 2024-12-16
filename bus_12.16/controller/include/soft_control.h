#ifndef SOFT_CONTROL_H
#define SOFT_CONTROL_H

#include "../../bus/include/softbus_new.h"
#include "../../include/discovery.h"
#include "../../../bridge/manager/include/bridge_manager.h"
#include "../../adapter/include/soft_adapter.h"



static int SoftBusControllerCounter = 0;     // 当前SoftBusController编号

typedef struct 
{
    int controllerId;
    SoftBusQueues *softBusList;   // 存储多个软总线
    SoftBusMutex lock;               // 用于锁定控制器操作 
} SoftBusController;


// 声明一个软总线控制器数组，假设最大软总线数量为 4
SoftBusController *globalSoftBusControllers[SOFTBUS_MAX_LISTS];

// 创建软总线控制器函数
SoftBusController *CreateSoftBusController();

int32_t AddComponentToQueue(int controllerId, PublishInfo *component);

int32_t DestroySoftBusController(int controllerId);

int32_t DestroyAllSoftBusController();

int32_t SoftControlInit();

int32_t SoftControlDeinit(int adapterId);

int32_t SoftControlAllDeinit();

#endif


