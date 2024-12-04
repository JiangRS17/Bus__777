#include "../include/bridge_manager.h"
#include "bridge_error.h"

//桥节点创建
int32_t BridgeInit(void)
{
    int32_t ret = BridgeMgrInit();
    if (ret != BRIDGE_OK) {
        printf("init fail");
        return ret;
    }
    return BRIDGE_OK;
}

//桥节点销毁
void BridgeDeinit(int bridgeId) 
{
    int32_t ret = BridgeMgrDeinit(bridgeId); 
    if (ret != BRIDGE_OK) {
        printf("destory fail");
        return ret;
    }
    return BRIDGE_OK;
}

// int32_t CreateBridge(const BridgeInfo *info) 
// {
//     int32_t ret = CreateMgrBridge(info);
//     if (ret != BRIDGE_OK)
//     {
//         printf("bridge node failed");
//         return ret;
//     }
//     printf("bridge node success!");

//     return BRIDGE_OK;
// }

// int32_t DestoryBridge(int32_t bridgeId)
// {
//     int32_t ret = DestoryMgrBridge(bridgeId);
//     if (ret != BRIDGE_OK)
//     {
//         printf("bridge node destory failed");
//         return ret;
//     }
//     printf("bridge node destory success");
//     return BRIDGE_OK;
    
// }
