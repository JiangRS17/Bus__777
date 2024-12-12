#include "softbus_new.c"
#include "bridge_manager.h"

int32_t SoftBusMgrInit(void) 
{
    SoftBusQueues *newlist = CreateSoftBusList();
    if (newlist == NULL)
    {
        printf("Fail");
        return SOFTBUS_ERR;
    }
    
    // 连接桥节点
    BridgeInit();

    return SOFTBUS_OK;
}

int32_t SoftBusMgrDeinit(int bridgeId)
{
    int32_t ret = DestroySoftBusList(bridgeId);
    if (ret)
    {
        return SOFTBUS_OK;
    }
    
    BridgeDeinit(bridgeId);
    
    return SOFTBUS_ERR;
}