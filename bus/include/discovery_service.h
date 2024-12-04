//discovery_service:声明统一设备服务发布接口。
/*
  该文件提供与设备服务发布相关的功能，包括发布服务、取消发布服务和设置设备参数。
  在发现多个附近的设备并将其联网后，可使用这些接口执行与服务发布相关的操作。
*/

#ifndef DISCOVERY_SERVICE_H
#define DISCOVERY_SERVICE_H

#include "softbus_list.h"
#include "softbus.h"
#include <stdint.h>



#define DISC_MAX_DEVICE_ID_LEN 96
#define DISC_MAX_DEVICE_NAME_LEN 65
#define DISC_MAX_CUST_DATA_LEN 219
#define DISC_MAX_CAPABILITY_NUM 2



typedef enum {
    AUTO = 0,   //automatic meduim selection (default)
}ExchangeMedium;

typedef enum {
    LOW = 0,
    MID = 1,
    HIGH = 2,
    SUPER_HIGH = 3
} ExchangeFreq;


//定义发送给发现设备的服务配置信息；
//service
typedef struct {
    int publishId;
    ExchangeMedium medium;
    ExchangeFreq freq;
    const char *capability;
    unsigned int *capabilityData;
    unsigned int dataLen;
}PublishInfo;

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


// typedef enum {
//     TYPE_1,
//     TYPE_2,
// } DeviceType;

// typedef enum {
//     // unsupported service;
//     DISCOVERY_FAIL_REASON_NOT_SUPPORT = 1,
//     //internal error;
//     DISCOVERY_FAIL_REASON_INTERNAL = 2,
//     //unknown reason;
//     DISCOVERY_FAIL_REASON_UNKNOWN = 0xFF
// } DiscoveryFailReason;

//device
// typedef struct {
//     char devId[DISC_MAX_DEVICE_ID_LEN];
//     // char accountHash[MAX_ACCOUNT_HASH_LEN];
//     DeviceType devType;
//     char devName[DISC_MAX_DEVICE_NAME_LEN];
//     unsigned int addrNum;
//     // ConnectionAddr addr[CONNECTION_ADDR_MAX];
//     // unsigned int capabilityBitmapNum;
//     // unsigned int capabilityBitmap[DISC_MAX_CAPABILITY_NUM];
//     // char custData[DISC_MAX_CUST_DATA_LEN];
// } DeviceInfo;


// //定义服务回调
// typedef struct {
//     // callback that is invoked when a device is found
//     void (*OnDeviceFound)(const DeviceInfo *device);
//     //failure
//     void (*OnDiscoverFailed)(int subscribeId, DiscoveryFailReason failReason);
//     //success
//     void (*OnDiscoverSuccess)(int subscribeId);
// } IDiscoveryCallback;

int PublishService(const char *pkgName, const PublishInfo *info, const IPulishCallback *cb);

int UnPublishService(const char *pkgName, int publishId);





#endif
