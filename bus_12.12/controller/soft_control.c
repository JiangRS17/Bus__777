#include "soft_control.h"
#include "softbus_error.h"
#include "discovery.h"

SoftBusController *CreateSoftBusController()
{
     // 为控制器分配内存
     SoftBusController *controller = (SoftBusController *)malloc(sizeof(SoftBusController));
     if (controller == NULL)
     {
        printf("Error: Failed to allocate memory for SoftBusController.\n");
        return SOFTBUS_ERR;
     }

    // 假设控制器的锁和队列是初始化好的
    pthread_mutex_init(&controller->lock, NULL);

    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++)
    {
        controller->softBusList[i] = NULL;  // 假设开始时没有软总线，或者以后会根据需要创建
    }
    
     controller->controllerId = SoftBusControllerCounter++;

    if (SoftBusControllerCounter <= SOFTBUS_MAX_LISTS)
    {
        globalSoftBusControllers[controller->controllerId] = controller;
    } else {
        printf("Error: Reached the maximum number of SoftBusControllers.\n");
        free(controller);
        return SOFTBUS_ERR;
    }
    
    printf("SoftBusController created with ID: %d\n", controller->controllerId);

    return controller;
}

// 将 PublishInfo 组件放到指定软总线的第一个队列中
int32_t AddComponentToQueue(int controllerId, PublishInfo *component)
{
    SoftBusController *controller = NULL;
    for (int i = 0; i < SoftBusControllerCounter; i++)
    {
        if (globalSoftBusControllers[i]->controllerId == controllerId)
        {
            controller = globalSoftBusControllers[i];
            break;
        }
        
    }
    
    if (controller == NULL)
    {
        printf("Error: Controller ID %d not found.\n", controllerId);
        return SOFTBUS_ERR;
    }
    

    // 锁定控制器
    pthread_mutex_lock(&controller->lock);

    // 获取指定软总线
    SoftBusQueues *bus = controller->softBusList[controllerId];
    // 获取第一个队列
    SoftBusList *firstQueue = &bus->queue[0];    

    // 将 PublishInfo 组件插入到第一个队列中
    if (firstQueue->cnt < MAX_COMPONENT_COUNT)
    {
        ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
        if (newNode == NULL) {
            printf("Error: Failed to allocate memory for ListNode.\n");
            pthread_mutex_unlock(&controller->lock);
            return SOFTBUS_ERR;
        }

        newNode->data = (void *)component;
        newNode->next = NULL;
        
        // 将节点插入队列
        ListAdd(&newNode,&firstQueue);
        firstQueue->cnt++;
        RegisterFuncCodeToBridge(controllerId,component);
    } else{
        printf("Error: Queue 0 on Bus %d is full.\n", controllerId);
    }


    // 解锁控制器
    pthread_mutex_unlock(&controller->lock);
    return SOFTBUS_OK;

}


//注册功能代码到桥节点
// 将 PublishInfo 注册到桥节点的功能代码映射表中。
int RegisterFuncCodeToBridge(int controllerId, PublishInfo *publishInfo)
{
    if (controllerId < 0 || controllerId >= SOFTBUS_MAX_LISTS) {
        printf("Error: Invalid controller ID.\n");
        return SOFTBUS_ERR; // 无效的控制器ID
    }

    BridgeInfo *bridge = globalBridegInfo[controllerId];
    uint32_t func_code = publishInfo->funCode.funcCode;

    // 检查是否已经存在该功能代码
    int hashIndex = HashFunc(func_code);
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        int idx = (hashIndex + i) % HASHMAP_SIZE;
        if (bridge->entries[idx].funcCode == func_code) {
            printf("Error: func_code %u already registered.\n", func_code);
            // pthread_mutex_unlock(&bridge->lock);
            return SOFTBUS_ERR; // 功能代码已注册
        }

        if (bridge->entries[idx].funcCode == 0) {
            // 找到空位，进行注册
            bridge->entries[idx].funcCode = func_code;
            bridge->entries[idx].info = publishInfo;
            printf("Registered func_code %u for controller %d.\n", func_code, controllerId);
            // pthread_mutex_unlock(&bridge->lock);
            return SOFTBUS_OK;
        }
    }

    // SoftBusMutexLock(&bridge->lock);
    // HashMapPut(bridge->funcCodeMap, func_code, publishInfo); // 将 funcCode 和 PublishInfo 关联
    // SoftBusMutexUnlock(&bridge->lock);
    return SOFTBUS_OK;
}

// 哈希函数
static int HashFunc(uint32_t funcCode) {
    return funcCode % HASHMAP_SIZE;
}
