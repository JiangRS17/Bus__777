#ifndef BRIDEGE_MANAGER_H
#define BRIDEGE_MANAGER_H

#include "../../bus/include/discovery_service.h"
#include <stdint.h>
#include<stdbool.h>

#define FUN_MAX_DEVICE_LD_LEN 256
#define BRIDEGE_MAX_NUM 10


// 全局桥节点数组，用于存储已创建的桥节点
extern BridgeInfo globalBridegInfo[BRIDEGE_MAX_NUM];

static bool isbridge = false;
// 全局变量，管理当前已创建的桥节点数量
static int bridgeCounter = 0;  // 当前桥节点的数量（用于管理bridgeId）


//Bridge 
typedef struct {
    int bridgeId;
    BusNumInfo busnum;
    FunctionCode func_code;
    BridgeType bridgeType;
    DevicePrivilege privilege;
} BridgeInfo;

typedef enum {
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
} DevicePrivilege;

typedef struct {
    int primary_bus_num;
    int secondary_bus_num;
    int sub_bus_num;
} BusNumInfo;

typedef struct {
    char funId[FUN_MAX_DEVICE_LD_LEN];
} FunctionCode;

typedef enum {
    TYPE_0,
    TYPE_1,
} BridgeType;

typedef enum {
    Bridge_FAIL_REASON_NOT_SUPPROT = 1,
    Bridge_FAIL_REASON_UNKNOWN = 0xFF
}BridgeFailReason;

typedef struct {
    void (*OnBridgeSucess)(int bridgeId);
    void (*OnBridgeFail)(int bridgeId, BridgeFailReason reason);
} IBridgeCallback;

// int CreateBridge(const BridgeInfo *info);

// int DestoryBridge(int bridgeId);

int BridgeMgrInit();

int BridgeMgrDeinit(int bridgeId);

BridgeInfo *CreateNewBridgeInfoNode();

int32_t DestroyBridgeInfoNode(int bridgeId);

#endif