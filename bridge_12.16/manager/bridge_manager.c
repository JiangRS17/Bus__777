#include "include/bridge_manager.h"
#include "../authentication/include/auth_manager.h"
#include "../transmission/include/trans_manager.h"

#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include <stdlib.h>

// static bool bg_isInited = false;
// static SoftBusList *bg_publishInfoList = NULL;


//创建一个新的发布Bridge节点
BridgeInfo *CreateNewBridgeInfoNode() 
{
    if (bridgeCounter >= SOFTBUS_MAX_LISTS) {
        printf("Error: Bridge limit reached, cannot create more nodes.\n");
        return NULL;
    }

    BridgeInfo *newBridgeInfo = (BridgeInfo *)malloc(sizeof(BridgeInfo));

    if (newBridgeInfo == NULL)
    {
        return NULL;
    }

    newBridgeInfo->bridgeId = bridgeCounter++;

    //如果这个软总线是第一个，那桥节点只连接一个软总线；否则，桥节点连接上下两个软总线
    // 初始化总线编号
    if (bridgeCounter == 1) {
        // 如果是第一个桥节点，仅连接一个软总线
        newBridgeInfo->busnum.primary_bus_num = 0;    // 假设从总线编号 0 开始
        newBridgeInfo->busnum.secondary_bus_num = -1; // 无次级总线
        newBridgeInfo->busnum.sub_bus_num = -1;       // 无子总线
    } else {
        // 如果不是第一个桥节点，连接上下两个软总线
        newBridgeInfo->busnum.primary_bus_num = bridgeCounter;
        newBridgeInfo->busnum.secondary_bus_num = bridgeCounter +1;
        newBridgeInfo->busnum.sub_bus_num = bridgeCounter +2;
    }

    newBridgeInfo->bridgeType = TYPE_0;
    newBridgeInfo->privilege = LOW;
    // 初始化功能代码映射表
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        newBridgeInfo->entries[i].funcCode = 0;  // 默认功能代码
        newBridgeInfo->entries[i].info = NULL;  // 无对应的 PublishInfo
    }

    globalBridegInfo[bridgeCounter-1] = newBridgeInfo;
    isbridge = true;
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
    if (bridgeId < 0 || bridgeId >= SOFTBUS_MAX_LISTS) {
        printf("Invalid bridge ID\n");
        return BRIDGE_ERR;
    }

    for (int i = 0; i < bridgeCounter; ++i) {
        if (globalBridegInfo[i]->bridgeId == bridgeId) {
            // 清空该桥节点信息
            globalBridegInfo[i]->bridgeId = -1;  // 或者设置为默认值
            globalBridegInfo[i]->busnum.primary_bus_num = -1;
            globalBridegInfo[i]->busnum.secondary_bus_num = -1;
            globalBridegInfo[i]->busnum.sub_bus_num = -1;
            // globalBridgeInfo[i].func_code.funId = -1;
            globalBridegInfo[i]->bridgeType = -1;
            globalBridegInfo[i]->privilege = -1;
        }

        // 释放计数器（将后续节点前移）
            for (int j = i; j < bridgeCounter - 1; ++j) {
                globalBridegInfo[j] = globalBridegInfo[j + 1];
            }

            bridgeCounter--;  // 更新桥节点计数器
            printf("Bridge node %d destroyed\n", bridgeId);

            return BRIDGE_OK;
    }
}
