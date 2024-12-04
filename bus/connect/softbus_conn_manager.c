


#include "softbus_conn_manager.h"
#include "discovery_service.h"
#include "softbus_error.h"
#include "softbus.c"

#include <stdint.h>
#include <stdio.h>


static SoftBusList *g_listenerList = NULL;
ConnectFuncInterface *g_connManager = 0;
#define CONNECT_TYPE_SHIFT 16


//???
#define OFF_SET_OF(type,member) ((size_t)&(((type *)0)->member))
//???
#define LIST_ENTRY(item,type,member) \
    ((type *)(void *)((char *)(item) - OFF_SET_OF(type,member))) \
//???
#define LISR_FOR_EACH_ENTRY(item,list,type,member) \
    for ((item) = LIST_ENTRY((list)->next,type,member); \
        &(item)->member != (list); \
        (item) = LIST_ENTRY((item)->member.next,type,member))



typedef struct TagConnListenerNode {
    ListNode node;
    ConnModule moduleId;
    ConnectCallback callback;
}ConnListenerNode;


// 添加一个监听
static int32_t AddListener(ConnModule moduleId, const ConnectCallback *callback)
{
    ConnListenerNode *item = NULL;
    ConnListenerNode *listNode = NULL;

    if (g_listenerList == NULL)
    {
        return SOFTBUS_ERR;
    }

    //lock
    //遍历整个链表
    LIST_FOR_EACH_ENTRY(listNode, &g_listenerList->list, ConnListenerNode, node ) {
        if(listNode->moduleId == moduleId) {
            //unlock
            return SOFTBUS_ERR;
        }
    }

    item = (ConnListenerNode *)SoftBusMalloc(sizeof(ConnListenerNode));

    if(item == NULL){
        printf("mallc fail\n");
        //unlock
        return SOFTBUS_ERR;
    }

    item->moduleId = moduleId;

//memcpy_s 函数是安全的内存拷贝函数，用于避免缓冲区溢出等内存错误
    if(memcpy_s(&(item->callback),sizeof(ConnectCallback),callback,sizeof(ConnectCallback)) !=0 ) {
        SoftBusFree(item);
        //unlock
        return SOFTBUS_ERR;
    }

    ListAdd(&(g_listenerList->list),&(item->node));
    g_listenerList -> cnt++;
    //unlock
    return SOFTBUS_OK;

}


// 删除一个监听
static void DelListener(ConnModule moduleId)
{
    ConnListenerNode *removeNode = NULL;
    if(g_listenerList == NULL) {
        printf("listenerList is NUll");
        return;
    }

    //lock
    LISR_FOR_EACH_ENTRY(removeNode,&g_listenerList,ConnListenerNode,node) {
        if(removeNode->moduleId == moduleId) {
            ListDelete(&(removeNode->node));
            SoftBusFree(removeNode);
            g_listenerList->cnt--;
            break;
        }
    }
    //unlock
    return;
}

//生成一个新的请求ID
uint32_t ConnGetNewRequestId(ConnModule moduleId)
{
#define REQID_MAX 1000000
    (void)moduleId;
    static uint32_t reqId = 1;
    reqId++;
    reqId = reqId % REQID_MAX + 1;
    return reqId;
}


static int32_t GetListenerByModuleId(ConnModule moudleId, ConnListenerNode *node)
{
    ConnListenerNode *listenerNode = NULL;

    if (g_listenerList == NULL)
    {
        return SOFTBUS_ERR;
    }
    
    int ret = SOFTBUS_OK;
    //lock
    LISR_FOR_EACH_ENTRY(listenerNode, &g_listenerList,ConnListenerNode,node){
        if(listenerNode->moduleId == moudleId) {
            if (memcpy_s(node,sizeof(ConnListenerNode),listenerNode,sizeof(ConnListenerNode)) != EOK)
            {
                ret = SOFTBUS_ERR;
            }
            //UNLOCK
            return ret;
        }
    }
    //unlock
    return SOFTBUS_ERR;
}

//连接管理器接收数据
void ConnManagerRecvData(uint32_t connectionId, ConnModule moduleId, int64_t seq, char *data, int32_t len) 
{
    ConnListenerNode listener;
    int32_t ret;
    char* ptr = NULL;
    int32_t pktlen;

    if (data == NULL)
    {
       return;
    }

    ret = GetListenerByModuleId(moduleId,&listener);
    if (ret == SOFTBUS_ERR)
    {
        printf("GetListenerByModuleId fail moduleId\n");
        return;
    }(PublishInfo *info, ExchangeFreq freq) 

    // pktlen = len - sizeof(ConnPkHead);
    // pkt = data + sizeof(ConnPkHead);
    listener.callback.OnDataReceived(connectionId,moduleId,seq,pkt,pktlen);
    return;
}

static int32_t GetALlListener(ConnListenerNode **node) 
{
    ConnListenerNode *listenerNode = NULL;
    int32_t cnt = 0;

    if(g_listenerList == NULL) {
        printf("listener list is null");
        return cnt;
    }

    if(g_listenerList->cnt == 0) {
        printf("listener cnt is null");
        return cnt;
    }

    //lock

    *node = SoftBusMalloc(g_listenerList->cnt * sizeof(ConnListenerNode));
    if(*node == NULL) {
        //UNLOCK
        return cnt;
    }

    LISR_FOR_EACH_ENTRY(listenerNode,&g_listenerList->list,ConnListenerNode,node) {
        if(memcpy_s(*node + cnt,sizeof(ConnListenerNode),listenerNode,sizeof(ConnListenerNode)) != EOK) {
            printf("mem error\n");
        }
        cnt++;
    }
    //unlock
    return cnt;
}


void ConnManagerConnected(uint32_t connectionId) {
    int32_t i,num;
    ConnListenerNode *node = NULL;
    ConnListenerNode *listener = NULL;

    num = GetAllListener(&node);

    if(num ==0 || node == NULL) {
        printf("get node fail connId\n");
        return;
    }

    for(i=0; i<num; i++) {
        listener = node + i;
        listener->callback.OnConnected(connectionId);
    }

    SoftBusFree(node);
    return;
}


void ConnManagerDisConnected(uint32_t connectionId) 
{
    int32_t i,num;
    ConnListenerNode *node = NULL;
    ConnListenerNode *listener = NULL;

    num = GetALlListener(&node);
    if (num == 0 || node == NULL)
    {
        return/* code */;
    }

    for ( i = 0; i < num; i++)
    {
        listener = node + i;
        listener->callback.OnDisConnected(connectionId);
    }
    
    SoftBusFree(node);
    return;
    
}

//根据连接选项获取相应管理器(ConnectOption)，调用管理器进行连接，并返回状态码
int32_t ConnConnectDevice(uint32_t requestId, const ConnectResult *result)
{
    return g_connManager->ConnectDevice(requestId,result);
}


int32_t ConnDisconnectDevice(uint32_t connectionId)
{
    // uint32_t type = (connectionId >> CONNECT_TYPE_SHIFT );

    return g_connManager->DisconnectDevice(connectionId);

}

int32_t ConnStartLocalListening(const LocalListenerInfo *info)
{
    if (info == NULL)
    {
        return SOFTBUS_INVALID_PARAM;
    }

    if (g_connManager->StartLocalListening == NULL)
    {
        return SOFTBUS_CONN_MANAGER_OP_NOT_SUPPORT;
    }
    
    return g_connManager->StartLocalListening(info);
    
}

int32_t ConnStopLocalListening(const LocalListenerInfo *info)
{
    if (info == NULL)
    {
        return SOFTBUS_INVALID_PARAM;
    }
    if(g_connManager->StopLocalListening == NULL)
    {
        return SOFTBUS_CONN_MANAGER_OP_NOT_SUPPORT;
    }

    return g_connManager->StopLocalListening(info);
    
}

//connection

static int32_t ConnectDevice(uint32_t requestId, const ConnectResult *result)
{
    printf("[ConnectDevice]");
    uint32_t connId = 0;
    int32_t ret = SOFTBUS_OK;
     
    //lock

    return ret;
}