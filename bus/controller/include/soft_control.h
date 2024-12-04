#ifndef SOFT_CONTROL_H
#define SOFT_CONTROL_H

#include "../../bus/include/softbus.h"
#include "../../include/discovery.h"


typedef struct {
    unsigned int controllerId;
    unsigned int totalDevices;  // 控制器管理的设备数量
    PublishInfo *devices;   // 设备列表
    
     // 初始化设备并分配唯一设备号
    int (*initialize)(struct SoftBusController *controller);

    // 数据传输函数
    int (*transferData)(struct SoftBusController *controller, unsigned int srcDeviceId, unsigned int destDeviceId, void *data, size_t size);

    // 中断处理函数
    int (*handleInterrupt)(struct SoftBusController *controller, unsigned int deviceId);

    // 总线控制和仲裁
    int (*arbitrate)(struct SoftBusController *controller, unsigned int requestingDeviceId);
} SoftBusController;


#endif


