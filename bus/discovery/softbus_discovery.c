#include "../include/softbus_error.h"
#include "../include/discovery.h"
#include "../bus/include/softbus.h"
#include "../bus/include/softbus_list.h"

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>



static PublishInfo *CreatePulishInfoNode ()
{
    PublishInfo *info = (PublishInfo *)malloc(sizeof(PublishInfo));
    if (info == NULL)
    {
        return NULL;
    }

    info->publishId = ComponentCout;
    ComponentCout ++;
    info->componentType = COMPONENT_TYPE_OTHER;    // 提供修改函数
    info->memoryRequirement;
    info->isActive = false;    // 提供修改函数
    info->isFaulty = false;
    info->freq = LOW;

    return info;
    
}

// // 向链表中添加新的 PublishInfo 节点
// static void AddPublishInfoTolist(PublishInfo *info)
// {

// }



// int32_t *DestroyPublishInfoNode(uint32_t publishId)
// {

// }



// 设置组件类型
void SetComponentType (PublishInfo *info, ComponentType type)
{
    if (info != NULL)
    {
        info->componentType = type;
    }
    
}

// 设置内存需求
//?????
void SetMemoryRequirement(PublishInfo *info, size_t memory) {
    if (info != NULL) {
        info->memoryRequirement = memory;
    }
}

// 设置是否激活
void SetIsActive(PublishInfo *info, bool isActive)
{
    if (info != NULL)
    {
        info->isActive = true;
    }
    
}

// 设置是否故障
void SetIsFaulty(PublishInfo *info, bool isFaulty) {
    if (info != NULL) {
        info->isFaulty = true;
    }
}

// 设置交换频率
void SetExchangeFreq(PublishInfo *info, ExchangeFreq freq) {
    if (info != NULL) {
        info->freq = freq;
    }
}