#include "soft_control.h"
#include "soft_adapter.h"
#include "soft_arbiter.h"
#include "softbus_error.h"

BusArbiter *CreateBusArbiter(SoftBusQueues *bus)
{
    BusArbiter *arbiter = (BusArbiter *)malloc(sizeof(BusArbiter));

    if (arbiter == NULL)
    {
        printf("Error: Failed to allocate memory for BusArbiter.\n");
        return SOFTBUS_ERR;
    }
    
    pthread_mutex_init(&arbiter->arbiterlock,NULL);
    arbiter->associatedBus = bus;
    arbiter->isBusOccupied = false;
    arbiter->currentBus = 0;

    return arbiter;
}


// 初始化全局仲裁器数组
void InitializeBusArbiters() {
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++) {
        if (globalSoftBusList[i] != NULL) {
            // 创建对应的仲裁器并存储
            BusArbiter *arbiter = CreateBusArbiter(globalSoftBusList[i]);
            if (arbiter != NULL) {
                globalBusArbiter[i] = arbiter;
                printf("BusArbiter for Bus %d initialized successfully.\n", i);
            } else {
                printf("Failed to initialize BusArbiter for Bus %d.\n", i);
            }
        }
    }
}


// 我们需要根据当前软总线是否被占用来决定是将请求放入队列 0 还是队列 1
int SoftBusArbiter_RequestBus(BusArbiter *arbiter, PublishInfo *publishInfo) 
{
    pthread_mutex_lock(&arbiter->arbiterlock);

    SoftBusList *queue0 = &arbiter->associatedBus->queue[0];
    SoftBusList *queue1 = &arbiter->associatedBus->queue[1];

    // 如果软总线已被占用，设备0
    if (arbiter->isBusOccupied)
    {
        pthread_mutex_unlock(&arbiter->arbiterlock);
        return SOFTBUS_ERR;
    } else{
        arbiter->isBusOccupied = true;
        int currentBus = &arbiter->currentBus;
        ///????
        int ret = RouteRequest(currentBus, publishInfo);
        arbiter->isBusOccupied = false;
        pthread_mutex_unlock(&arbiter->arbiterlock);
        return SOFTBUS_OK;
    } 
}


int32_t HandlerClockInterrupt(int currentBus)
{
    BusArbiter *arbiter = globalBusArbiter[currentBus];

    if (arbiter == NULL)
    {
        printf("Error: BusArbiter for Bus %d not found.\n", currentBus);
        return SOFTBUS_ERR;
    }

    pthread_mutex_lock(&arbiter->arbiterlock);

    // 检查是否有任务正在执行
    if (!arbiter->isBusOccupied) {
        printf("No task is currently executing on bus %d.\n", currentBus);
        pthread_mutex_unlock(&arbiter->arbiterlock);
        return SOFTBUS_ERR;
    }
    
    // 获取队列信息
    SoftBusList *queue0 = &arbiter->associatedBus->queue[0]; 
    SoftBusList *queue1 = &arbiter->associatedBus->queue[1]; 

    // 从队列1中取出当前正在执行的任务
    if (!IsListEmpty(&queue1->list))
    {
        ListNode *currentTaskNode = queue1->list.next;
        PublishInfo *currentTask = (PublishInfo *)currentTaskNode->data;

        // 保存任务上下文（这里假设上下文包含任务基本信息）
        SaveTaskContext(currentTask);

        // 将任务移到队列0
        ListDelete(currentTaskNode);
        ListAdd(&queue0->list, currentTaskNode);

        queue1->cnt--;
        queue0->cnt++;
    }

    arbiter->isBusOccupied = false;

    pthread_mutex_unlock(&arbiter->arbiterlock);

    return SOFTBUS_OK;
    
}

// 保存任务上下文
void SaveTaskContext(PublishInfo *task)
{
    
}

//中断触发机制（定时触发）
void TriggerClockInterrupt(int busId)
{
    printf("Clock interrupt triggered on bus %d.\n", busId);
    ClockInterruptHandler(busId);
}