#include "../include/softbus_error.h"
#include "../include/discovery.h"
#include "../bus/include/softbus_new.h"
#include "../bus/include/softbus_list.h"

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>


// PublishInfo *CreatePublishInfoNode(PublishInfoInitCallback callback) 
PublishInfo *CreatePulishInfoNode ()
{
    PublishInfo *info = (PublishInfo *)malloc(sizeof(PublishInfo));
    if (info == NULL)
    {
        return NULL;
    }

    info->publishId = ComponentCout;
    ComponentCout ++;
    info->componentType = COMPONENT_TYPE_OTHER;    // 提供修改函数
    info->isActive = false;    // 提供修改函数
    info->isFaulty = false;
    info->freq = INFO_LOW;
    info->memoryRequirement = CalculateMemoryRequirement(info->componentType);

    info->funCode.funcCode = 0;
    info->funCode.requestId = 0;
    strncpy(info->funCode.content, "Default Message", sizeof(info->funCode.content));
    return info;
    
}


// 根据组件类型和通信频率计算内存需求
size_t CalculateMemoryRequirement(ComponentType type) {
    size_t baseMemory = 0;

    // 按组件类型设置基准内存
    switch (type) {
        case COMPONENT_TYPE_ACTUATOR :
            baseMemory = 2 * PAGE_SIZE; // 8 KB
            break;
        case COMPONENT_TYPE_COMPUTE:
            baseMemory = 4 * PAGE_SIZE; // 16 KB
            break;
        case COMPONENT_TYPE_STORAGE:
            baseMemory = 8 * PAGE_SIZE; // 32 KB
            break;
        default:
            baseMemory = PAGE_SIZE; // 默认分配 4 KB
            break;
    }

    // 确保内存需求按页大小对齐
    size_t alignedMemory = ((baseMemory + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    return alignedMemory;
}


void SetFuncCode(Message *message, uint32_t newFuncode)
{
    message->funcCode = newFuncode;
}

void SetRequestId(Message *message, uint32_t newRequestId)
{
    message->requestId = newRequestId;
}


// 设置组件类型
void SetComponentType (PublishInfo *info, ComponentType type)
{
    if (info != NULL)
    {
        info->componentType = type;
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


