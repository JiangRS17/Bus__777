#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "softbus_list.h"
#include "softbus.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


static int ComponentCout = 0;     // 当前Component编号

// 定义组件类型的枚举
typedef enum {
    COMPONENT_TYPE_SENSOR,
    COMPONENT_TYPE_ACTUATOR,
    COMPONENT_TYPE_COMPUTE,
    COMPONENT_TYPE_STORAGE,
    COMPONENT_TYPE_OTHER
} ComponentType;


typedef enum {
    LOW = 0,
    MID = 1,
    HIGH = 2,
    SUPER_HIGH = 3
} ExchangeFreq;


typedef struct 
{
    uint32_t publishId;
    ComponentType componentType;  
    size_t memoryRequirement;         // 组件需要的内存空间 (字节)
    bool isActive;                                // 是否激活
    bool isFaulty;                                // 是否存在故障
    ExchangeFreq freq;
    // char componentName[MAX_COMPONENT_NAME_LEN];   // 组件名称
} PublishInfo;

typedef enum {
    // unsupported service;
    PUBLISH_FAIL_REASON_NOT_SUPPORT = 1,
    //internal error;
    PUBLISH_FAIL_REASON_INTERNAL = 2,
    //unknown reason;
    PUBLISH_FAIL_REASON_UNKNOWN = 0xFF
}PublishFailReason;

//定义服务发送成功和失败的回调
typedef struct {
    //callback successful
    void (*OnPulishSuccess)(int pulishId);
    //callback failed
    void (*OnPulishFail)(int pulishId, PublishFailReason reason);
}IPulishCallback;



#endif
