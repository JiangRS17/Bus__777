#ifndef SOFT_CONTROL_H
#define SOFT_CONTROL_H

#include "../../../bridge/manager/include/bridge_manager.h"
#include "../../adapter/include/soft_adapter.h"



extern int SoftBusControllerCounter;     // 当前SoftBusController编号

typedef struct 
{
    int controllerId;
    SoftBusQueues *softBusList;   // 存储多个软总线
    SoftBusMutex lock;               // 用于锁定控制器操作 
} SoftBusController;


// 声明一个软总线控制器数组，假设最大软总线数量为 4
extern SoftBusController *globalSoftBusControllers[SOFTBUS_MAX_LISTS];

// 创建软总线控制器函数
extern SoftBusController *CreateSoftBusController();

extern int32_t AddComponentToQueue(int controllerId, PublishInfo *component);

extern int32_t DestroySoftBusController(int controllerId);

extern int32_t DestroyAllSoftBusController();

extern int32_t SoftControlInit();

extern int32_t SoftControlDeinit(int adapterId);

extern int32_t SoftControlAllDeinit();

#endif


