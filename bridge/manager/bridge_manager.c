#include "../include/bridge_manager.h"
#include "../bridge_error.h"
#include "softbus_new.h"
#include "../../bus/include/softbus_error.h"
#include "../../bus/bus/include/softbus.h"

#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>


// static bool bg_isInited = false;
// static SoftBusList *bg_publishInfoList = NULL;

BridgeInfo globalBridgeInfo[BRIDEGE_MAX_NUM];


//创建一个新的发布Bridge节点
static BridgeInfo *CreateNewBridgeInfoNode() 
{
    int32_t ret;
    
    // BridgeInfo *newBridgeInfo = (BridgeInfo *)SoftBusMalloc(sizeof(BridgeInfo));
    BridgeInfo *newBridgeInfo = &globalBridgeInfo[bridgeCounter];

    if (newBridgeInfo == NULL){
        printf("malloc infoNode failed\n");
        return NULL;
    }

    int bridgeId = bridgeCounter;
    bridgeCounter++;

    //如果这个软总线是第一个，那桥节点只连接一个软总线；否则，桥节点连接上下两个软总线
    BusNumInfo busnum = {busCounter, busCounter + 1, busCounter + 2};
    busCounter++;

    FunctionCode func_code = {0};  // 示例功能代码
    BridgeType bridgeType = TYPE_0;    // 示例桥节点类型
    DevicePrivilege privilege = LOW;  // 权限级别

    newBridgeInfo->bridgeId = bridgeId; 
    newBridgeInfo->busnum = busnum;
    newBridgeInfo->func_code = func_code;
    newBridgeInfo->bridgeType = bridgeType;
    newBridgeInfo->privilege = privilege;

    globalBridgeInfo[bridgeCounter].bridgeId = bridgeId;      // 使用传入的桥节点 ID
    globalBridgeInfo[bridgeCounter].busnum = busnum;          // 直接赋值传入的总线编号
    globalBridgeInfo[bridgeCounter].func_code = func_code;    // 直接赋值功能代码
    globalBridgeInfo[bridgeCounter].bridgeType = bridgeType;  // 直接赋值桥节点类型
    globalBridgeInfo[bridgeCounter].privilege = privilege;    // 直接赋值权限
    
    // return &globalBridgeInfo[bridgeCounter];
    return newBridgeInfo;
}

int32_t BridgeMgrInit(void) 
{
    //前面判断过
    // // 判断上下软总线是否建立
    // int32_t ret = CreateSoftBusList();
    // if (ret != SOFTBUS_OK)
    // {
    //     printf("softbus init failed");
    //     return SOFTBUS_ERR;
    // }

    // 调用CreateNewBridgeInfoNode函数创建新桥节点
    BridgeInfo *newBridge = CreateNewBridgeInfoNode();

    if (newBridge == NULL) {
        printf("Fail");
        return BRIDGE_ERR;
    }

    isbridge = true;

    return BRIDGE_OK;
}



int32_t BridgeMgrDeinit(int bridgeId) 
{
    int32_t ret = DestroyBridgeInfoNode(bridgeId); 
    printf("Bridge Manager deinitialized\n");

    isbridge = false;

    //总线收回
    // int32_t ret_bus = SoftBusMgrDeinit();
    // printf("Bus Manager deinitialized\n");

    // if (ret==BRIDGE_OK && ret_bus == SOFTBUS_OK) {
    //     return BRIDGE_OK;
    // }

    return BRIDGE_ERR;
}



// 销毁一个桥节点
// !!!!!!!!!!这里逻辑需要改，收回节点的时候，需要收回下面挂载的所有!!!!!!!!!!!!
int32_t DestroyBridgeInfoNode(int bridgeId) 
{
    if (bridgeId < 0 || bridgeId >= BRIDEGE_MAX_NUM) {
        printf("Invalid bridge ID\n");
        return BRIDGE_ERR;
    }

    for (int i = 0; i < bridgeCounter; ++i) {
        if (globalBridgeInfo[i].bridgeId == bridgeId) {
            // 清空该桥节点信息
            globalBridgeInfo[i].bridgeId = -1;  // 或者设置为默认值
            globalBridgeInfo[i].busnum.primary_bus_num = -1;
            globalBridgeInfo[i].busnum.secondary_bus_num = -1;
            globalBridgeInfo[i].busnum.sub_bus_num = -1;
            globalBridgeInfo[i].func_code.funId = -1;
            globalBridgeInfo[i].bridgeType = -1;
            globalBridgeInfo[i].privilege = -1;
        }

        // 释放计数器（将后续节点前移）
            for (int j = i; j < bridgeCounter - 1; ++j) {
                globalBridgeInfo[j] = globalBridgeInfo[j + 1];
            }

            bridgeCounter--;  // 更新桥节点计数器
            printf("Bridge node %d destroyed\n", bridgeId);

            return BRIDGE_OK;
    }
}


// int32_t CreateMgrBridge(const BridgeInfo *info)
// {
//     int32_t ret; 
//     // 判断上下软总线是否建立
//     if(info == NULL){
//         return BRIDGE_ERR;
//     }

//     BridgeInfo *infoNode = CreateNewBridgeInfoNode(info);
//     if(infoNode == NULL) {
//         printf("infoNode create failed\n");
//         ret = BRIDGE_ERR;
//         return ret;
//     }

//     return BRIDGE_OK;
// }


// int32_t DestoryMgrBridge(int32_t bridgeId)
// {
//     BridgeInfo *infoNode = DeleteIntoAllFromList(bg_publishInfoList,bridgeId);
//     if(infoNode == NULL)
//     {
//         printf("delete softbus list fail");
//         return BRIDGE_ERR;
//     }
//     DestroySoftBusList(bg_publishInfoList);
// }


//桥节点创建好，即与上层软总线连接好