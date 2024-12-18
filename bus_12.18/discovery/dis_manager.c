#include "../include/discovery.h"

// //初始化一个设备管理器
PublishInfo *PublishMgrInit(void)
{
    PublishInfo *ret = CreatePulishInfoNode();
    if (ret == NULL )
    {
        return NULL;
    }

    return ret;
    
}


void ComponentMemMgr(ComponentType type)
{
    CalculateMemoryRequirement(type);
}


void ComponentTypeMgr (PublishInfo *info, ComponentType type)
{
    SetComponentType(info, type);
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
