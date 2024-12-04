#include "softbus_discovery.c"

// //初始化一个设备管理器
int32_t PublishMgrInit(void)
{
    int ret = CreatePulishInfoNode();
    if (ret == NULL )
    {
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
    
}



//destroy 一个设备管理器
// int32_t PublishMgrDeinit(uint32_t publishId)
// {
//     int ret = DestroyPublishInfoNode(publishId);
//     if (ret == SOFTBUS_OK)
//     {
//        return SOFTBUS_OK;
//     }

//     return SOFTBUS_ERR;
    
// }


void ComponentTypeMgr (PublishInfo *info, ComponentType type)
{
    SetComponentType(info, type);
}

// 设置内存需求
void MemoryRequirementMgr (PublishInfo *info, size_t memory)
{
    SetMemoryRequirement(info, memory);
}

void IsActiveMgr(PublishInfo *info, bool isActive)
{
    SetIsActive(info, isActive);
}

void IsFaultyMgr (PublishInfo *info, bool isFaulty)
{
    SetIsFaulty(info, isFaulty);
}

void ExchangeFreqMgr (PublishInfo *info, ExchangeFreq freq) 
{
    SetExchangeFreq(info, freq);
}