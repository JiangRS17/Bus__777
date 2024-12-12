#include "soft_adapter.h"
#include "softbus_error.h"
#include "discovery.h"
#include "trans_manager.h"
#include "bridge_error.h"

SoftBusAdapter *CreateSoftBusAdapter(int adapterId)
{

    if (globalAdapterCount >= SOFTBUS_MAX_LISTS)
    {
        printf("Error: Maximum number of adapters reached.\n");
        return NULL;
    }
    
    SoftBusAdapter *adapter = (SoftBusAdapter *)malloc(sizeof(SoftBusAdapter));

    if (adapter == NULL)
    {
        printf("Error: Failed to allocate memory for SoftBusAdapter.\n");
        return SOFTBUS_ERR;
    }
    
    adapter->adapterId = adapterId;
    // adapter->associatedBus[adapterId] = NULL; // 初始时没有绑定软总线
    adapter->isAvailable = true;

    // 将所有关联的软总线指针初始化为 NULL
    // 需不需要调用对应初始化的函数
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++) {
        adapter->associatedBus[i] = NULL;
    }

    for (int i = 0; i < BRIDEGE_MAX_NUM; i++)
    {
        adapter->associatedBus[i] = NULL;
    }
    
    // 初始化适配器锁
    if (pthread_mutex_init(&adapter->adapterLock, NULL) != 0) {
        printf("Error: Failed to initialize adapter lock.\n");
        free(adapter);
        return NULL;
    }

    globalAdapter[globalAdapterCount++] = adapter;

    printf("SoftBusAdapter with ID %d created successfully.\n", adapterId);
    return adapter;
}




//从队列 0中取出请求 to queue 1，并将其交给适配器和组件进行处理。
int ProcessPublishInfoRequest(int adapterId)
{
    // 查找指定适配器
    SoftBusAdapter *adapter = NULL;
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++)
    {
        if (globalAdapter[i] != NULL && globalAdapter[i]->adapterId = adapterId)
        {
            adapter = globalAdapter[i];
            break;
        }
        
    }

    if (adapter == NULL)
    {
        printf("Error: Adapter ID %d not found.\n", adapterId);
        return SOFTBUS_ERR;
    }

    SoftBusList *firstQueue = &adapter->associatedBus[adapterId]->queue[0];

    // 加锁以确保线程安全
    pthread_mutex_lock(&firstQueue->lock); 

    if (firstQueue->cnt > 0)
    {
         // 获取队列中的第一个元素
        ListNode *node = firstQueue->list.next; // 假设是FIFO队列，头部是最早加入的节点
        PublishInfo *publishInfo = (PublishInfo *)node->data;

         // 从队列中移除该元素
        list_del(&node);
        firstQueue->cnt--;

        // 处理该请求
        if (publishInfo->isActive && !publishInfo->isFaulty) {
            // 根据发布信息的 FunctionCode 路由到正确的组件
            printf("Processing PublishInfo ID for Component: \n");
            
            // 处理逻辑：路由请求并发送到对应的组件
            // RouteToComponent(adapterId, publishInfo);
            RouteRequest(adapterId,publishInfo);
        } else {
            printf("PublishInfo ID %d is inactive or faulty, skipping processing.\n", publishInfo->publishId);
        }

        // 解锁
        pthread_mutex_unlock(&firstQueue->lock);

        return SOFTBUS_OK;

    }

    // 解锁
    pthread_mutex_unlock(&firstQueue->lock);
    printf("Queue 1 is empty, no requests to process.\n");
    return SOFTBUS_ERR; // 没有请求处理
}


//适配器接收 PublishInfo 请求，并尝试将其插入到关联软总线的目标队列中
//适配器初始化时分配内存并绑定软总线，默认无桥节点。
int AdapterSendRequest(SoftBusAdapter *adapter, PublishInfo *publishInfo, BridgeInfo *bridge)
{
    
   
    SoftBusQueues *target = adapter->associatedBus;

    SoftBusList *targetQueue = &target->queue[1];

    // pthread_mutex_lock(&adapter->adapterLock);
    pthread_mutex_lock(&targetQueue->lock);


    if (targetQueue < SOFTBUS_MAX_QUEUES)
    {
        ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));

        if (newNode == NULL)
        {
            printf("Error: Memory allocation failed.\n");
            pthread_mutex_unlock(&targetQueue->lock);
            // pthread_mutex_unlock(&adapter->adapterLock);
            return SOFTBUS_ERR;
        }

        newNode->data = (void *)publishInfo;
        newNode->next = NULL;

        ListAdd(&newNode,&targetQueue);
        targetQueue++;
        
        ProcessComponentRequest(bridge, publishInfo);
    } else {
        printf("Error: Queue is full on Adapter %d.\n", adapter->adapterId);
    }
    
    pthread_mutex_unlock(&targetQueue->lock);
    pthread_mutex_unlock(&adapter->adapterLock);

    return SOFTBUS_OK;
}

//控制器根据请求的目标软总线和适配器状态选择适配器进行请求处理。
int RouteRequest(int adapterId, PublishInfo *publishInfo)
{

    // 查找指定 ID 的适配器
    SoftBusAdapter *adapter = NULL;
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++) {
        if (globalAdapter[i] != NULL && globalAdapter[i]->adapterId == adapterId) {
            adapter = globalAdapter[i];
            break;
        }
    }

    if (adapter == NULL || publishInfo == NULL) {
        printf("Error: Adapter or requested FunctionCode is null.\n");
        return SOFTBUS_ERR;
    }

    pthread_mutex_lock(&adapter->adapterLock);

    for (int i = 0; i < adapter->adapterId; i++)
    {
        BridgeInfo *bridge = adapter->bridges[i];
        int requestId = publishInfo->funCode.requestId;
        //访问桥节点设备表，是否存在对应请求ID(包括权限验证)
        int ret = ReadBridgeDeviceTabl(bridge,requestId);
        if (ret == BRIDGE_OK)
        {
            int ret = AdapterSendRequest(adapter,publishInfo,bridge);
            pthread_mutex_unlock(&adapter->adapterLock);
            printf("Request routed to Bridge ID %d on Bus %d.\n",bridge->bridgeId, bridge->busnum);
            return SOFTBUS_OK;
        }
        
    }

    pthread_mutex_unlock(&adapter->adapterLock);
    printf("Error: No matching bridge found for FunctionCode.\n");
    return SOFTBUS_ERR;
}


//将请求发送到目标组件，具体实现根据组件类型可能有所不同。
void ProcessComponentRequest(BridgeInfo *bridge, PublishInfo *publishInfo) {
    // 根据桥节点类型与组件类型，执行具体的处理逻辑
    printf("Processing PublishInfo ID  in Component with FunctionCode .\n");
    
    // 这里可以具体化为不同组件的处理逻辑
    // 例如：如果是 CPU 组件，则执行计算；如果是 IO 组件，则执行通信等
    //.........
}



int32_t DestroySoftBusAdapter(int adapterId)
{
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++)
    {
        if (globalAdapter[i] !=NULL && globalAdapter[i]->adapteId == adapterId)
        {
            pthread_mutex_destroy(&globalAdapter[i]->adapterLock);

            // 清理桥节点引用
            for (int j = 0; j < BRIDEGE_MAX_NUM; j++) {
                globalAdapter[i]->associatedBus[i] = NULL;
                globalAdapter[i]->bridges[j] = NULL; // 假设桥节点的释放由其他模块管理
            }

            free(globalAdapter[i]);
            globalAdapter[i] = NULL;

            globalAdapterCount--;

            return SOFTBUS_OK;
        }
        
    }

     // 未找到指定适配器
    printf("Error: SoftBusAdapter ID %d not found.\n", adapterId);
    return SOFTBUS_ERR;
}

int32_t DestroyAllSoftBusAdapter()
{
    for (int i = 0; i < SOFTBUS_MAX_LISTS; i++)
    {
        if (globalAdapter[i] != NULL)
        {
            printf("Destroying Adapter ID %d.\n", globalAdapter[i]->adapterId);
            DestroySoftBusAdapter(globalAdapter[i]);
            globalAdapter[i] = NULL; // 清空全局数组中的指针
        }
        
    }

    globalAdapterCount = 0; // 重置适配器计数
    printf("All SoftBusAdapters destroyed successfully.\n");
    
    return SOFTBUS_OK;
    
}



