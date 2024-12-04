#include "soft_control.h"

// 初始化函数：初始化总线上的所有设备，并分配唯一设备号
int initialize(SoftBusController *controller) {
    if (controller == NULL) {
        return -1;  // 错误：控制器为空
    }

    // 初始化设备
    for (unsigned int i = 0; i < controller->totalDevices; i++) {
        PublishInfo *device = &controller->devices[i];
        device->deviceId = i;  // 为每个设备分配唯一ID
        device->isActive = true;  // 激活设备
        snprintf(device->deviceName, sizeof(device->deviceName), "Device_%u", device->deviceId);
        printf("Device initialized: ID = %u, Name = %s\n", device->deviceId, device->deviceName);
    }

    return 0;  // 初始化成功
}

// 数据传输函数：设备间进行数据传输
int transferData(SoftBusController *controller, unsigned int srcDeviceId, unsigned int destDeviceId, void *data, size_t size) {
    if (controller == NULL) {
        return -1;  // 错误：控制器为空
    }

    // 校验设备ID
    if (srcDeviceId >= controller->totalDevices || destDeviceId >= controller->totalDevices) {
        return -1;  // 错误：设备ID无效
    }

    // 获取总线锁，防止同时传输
    pthread_mutex_lock(&controller->lock);

    // 传输数据（这里只是模拟数据传输）
    printf("Transferring data from Device %u to Device %u. Data size: %zu bytes.\n", srcDeviceId, destDeviceId, size);

    // 释放锁，允许其他设备访问总线
    pthread_mutex_unlock(&controller->lock);

    return 0;  // 数据传输成功
}

// 中断处理函数：设备发生中断时进行处理
int handleInterrupt(SoftBusController *controller, unsigned int deviceId) {
    if (controller == NULL || deviceId >= controller->totalDevices) {
        return -1;  // 错误：控制器为空或设备ID无效
    }

    // 处理中断（这里只是模拟中断处理）
    printf("Handling interrupt for Device %u.\n", deviceId);

    return 0;  // 中断处理成功
}

// 总线仲裁函数：处理设备访问总线的请求
int arbitrate(SoftBusController *controller, unsigned int requestingDeviceId) {
    if (controller == NULL || requestingDeviceId >= controller->totalDevices) {
        return -1;  // 错误：控制器为空或设备ID无效
    }

    // 进行仲裁（这里只是模拟仲裁过程）
    printf("Arbitrating access to bus for Device %u.\n", requestingDeviceId);

    // 假设仲裁成功
    return 0;  // 仲裁成功
}

// 创建并初始化 SoftBusController
SoftBusController *createController(unsigned int totalDevices) {
    SoftBusController *controller = (SoftBusController *)malloc(sizeof(SoftBusController));
    if (controller == NULL) {
        return NULL;  // 错误：内存分配失败
    }

    controller->controllerId = 1;  // 假设控制器ID为1
    controller->totalDevices = totalDevices;
    controller->devices = (SoftBusDevice *)malloc(sizeof(SoftBusDevice) * totalDevices);

    if (controller->devices == NULL) {
        free(controller);
        return NULL;  // 错误：内存分配失败
    }

    // 初始化控制器的各个功能
    controller->initialize = initialize;
    controller->transferData = transferData;
    controller->handleInterrupt = handleInterrupt;
    controller->arbitrate = arbitrate;

    // 初始化控制器的锁
    pthread_mutex_init(&controller->lock, NULL);

    return controller;
}