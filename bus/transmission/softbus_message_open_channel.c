
#include <stdint.h>
#include <stdio.h>

#include "cJSON.h"
#include "discovery_service.h"
#include "softbus_transmission.h"
#include "softbus_error.h"
#include "softbus.c"
#include "softbus_conn_manager.c"
#include "session.h"


#define TIME_OUT 2

//将错误代码和描述信息打包成JSON格式并返回
char *PackError(int errCode, const char *errDesc) 
{
    if(errDesc == NULL) {
        printf("invalid param");
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    if(json == NULL) {
        printf("cannot create json object");
        return NULL;
    }

    char *data = cJSON_PrintUnformatted(json);
    if (data == NULL)
    {
        printf("cJSON_PrintUnformatted failed");
    }

    cJSON_Delete(json);
    return data;
    
}

//等待包信息
typedef struct {
    ListNode node;
    SoftBusCond cond;
    int32_t channelId;
    int32_t seq;
    uint8_t status;
} PendingPktInfo;

enum PackageStatus {
    PACKAGE_STATUS_PENDING = 0,
    PACKAGE_STATUS_FINISHED,
    PACKAGE_STATUS_CANCELED,
};

static SoftBusList *g_pendingList[PENDING_TYPE_BUFF] = {NULL,NULL};

//初始化一个等待链表
int32_t PendingInit(int type)
{
    if (type < PENDING_TYPE_PROXY || type >= PENDING_TYPE_BUFF)
    {
        return SOFTBUS_ERR;
    }

    g_pendingList[type] = CreateSoftBusList();
    if (g_pendingList[type] == NULL)
    {
        /* code */
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
}

////销毁一个等待链表
void PendingDeinit(int type) 
{
    if (type < PENDING_TYPE_PROXY || type >= PENDING_TYPE_BUFF)
    {
        return ;
    }

    if(g_pendingList[type] != NULL) {
        DestroySoftBusList(g_pendingList[type]);
        g_pendingList[type] = NULL;
    }

    printf("pending package manager deinit init ok");

}

// //创建一个待处理的数据包对象，进行初始化//处理一个等待的包
static PendingPktInfo *CreatePendingItem(int32_t channelId, int32_t seqNum)
{
    PendingPktInfo *item = (PendingPktInfo *)SoftBusMalloc(sizeof(PendingPktInfo));
    if (item == NULL)
    {
        return NULL;
    }

    //lock
    SoftBusCondInit(&item->cond);
    item->seq = seqNum;
    item->status = PACKAGE_STATUS_PENDING;
    return item;
    
}

//释放一个资源
static void ReleasePendingItem(PendingPktInfo *item)
{
    if(item == NULL){
        return;
    }

    //UNLOCK
    SoftBusCondDestroy(&item->cond);
    SoftBusFree(item);
}

//s处理一个等待的包
int32_t ProcPendingPacket(int32_t channelId, int32_t seqNum, int type)
{
    if (type < PENDING_TYPE_PROXY || type >= PENDING_TYPE_BUFF)
    {
        return SOFTBUS_ERR;
    }

    PendingPktInfo *item;
    SoftBusList *pendingList = g_pendingList[type];
    if (pendingList == NULL)
    {
        return SOFTBUS_ERR;
    }
    
    //LOCK
    LISR_FOR_EACH_ENTRY(item,&pendingList->list, PendingPktInfo, node)
    {
        if (item->seq == seqNum && item->channelId == channelId)
        {
            printf("pendingpacket already created");
            //unlock
            return SOFTBUS_ERR;
        }
        
    }

    item = CreatePendingItem(channelId,seqNum);
    if (item == NULL)
    {
        //unlock
        return SOFTBUS_MALLOC_ERR;
    }
    ListAdd(&pendingList->list, &item->node);
    pendingList->cnt++;
    //unlock;

    SoftBusSysTime outtime;
    SoftBusSysTime now;

    SoftBusGetTime(&now);

    outtime.sec = now.sec + TIME_OUT;
    outtime.usec = now.usec;

    //lock

    while (item->status == PACKAGE_STATUS_PENDING )
    {
        //>????
        SoftBusCondWait(&item->cond,&outtime);
    }

    int32_t ret = SOFTBUS_OK;
    if(item->status != PACKAGE_STATUS_FINISHED) {
        printf("time out");
        return SOFTBUS_ERR;
    }


    //unlock
    //lock
    ListDelete(&item->node);
    pendingList->cnt--;
    //unlock
    ReleasePendingItem(item);

    return ret;
    

}

////创建一个等待的包
int32_t SetPendingPacket(int32_t channelId, int32_t seqNum, int type)
{
    if (type < PENDING_TYPE_PROXY || type >= PENDING_TYPE_BUFF) {
        return SOFTBUS_ERR;
    }

    SoftBusList *pendingList = g_pendingList[type];
    if (g_pendingList == NULL)
    {
        printf("pending list not exist");
        return SOFTBUS_ERR;
    }

    PendingPktInfo *item = NULL;
    //lock
    LISR_FOR_EACH_ENTRY(item,&pendingList->list, PendingPktInfo,node) {
        if(item->seq == seqNum && item->channelId == channelId) {
            item->status = PACKAGE_STATUS_FINISHED;
            // SoftBUsConSignal(&item->cond);
            //unlock
            return SOFTBUS_OK;
        }
    }

    //unlock
    return SOFTBUS_ERR;
    
}

////删除一个等待的包
int32_t DelPendingPacket(int32_t channelId, int type)
{
    if (type < PENDING_TYPE_PROXY || type >= PENDING_TYPE_BUFF) {
        return SOFTBUS_ERR;
    }

    SoftBusList *pendingList = g_pendingList[type];
    if (pendingList == NULL) {
        return SOFTBUS_ERR;
    }

    PendingPktInfo *item = NULL;
    //lock
    LISR_FOR_EACH_ENTRY(item,&pendingList->list,PendingPktInfo,node) {
        if(item->channelId == channelId) {
            item->status = PACKAGE_STATUS_CANCELED;
            //SoftBusCondSignal(&item->cond);
            //unlock
            return SOFTBUS_OK;
        }
    }
    //unlock
    return SOFTBUS_ERR;
}


////client_trans_message_service

int SendBytes(int sessionId, const void *data, unsigned int len)
{
    printf("sendbytes: sessionId = %d",sessionId);

    if (data == NULL || len == 0)
    {
        printf("invalid param");
        return SOFTBUS_INVALID_PARAM;
    }

    uint32_t maxLen;

    //????
    if (SoftBusGetConfig(SOFTBUS_INT_MAX_BYTES_LENGTH,(unsigned char *)&maxLen, sizeof(maxLen)) != SOFTBUS_OK) {
        return SOFTBUS_GET_CONFIG_VAL_ERR;
    }

    if(len > maxLen) {
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t channelId = INVALID_CHANNEL_ID;
    int32_t type = CHANNEL_TYPE_BUTT;
    bool isEnable = false;

    if (ClientGetChannelBySessionId(sessionId, &channelId, &type,&isEnable) != SOFTBUS_OK)
    {
        return  SOFTBUS_TRANS_INVALID_SEESION_ID;
    }
    
    if (isEnable != true)
    {
        return SOFTBUS_TRANS_SESSION_OPENING;
    }
    
    //???
    return ClientTransChannelSendBytes(channelId, type, data,len);
    
}

//sendmessage
int SendMessage(int sessionId, const void *data, unsigned int len)
{
    printf("sendbytes: sessionId = %d",sessionId);

    if (data == NULL || len == 0)
    {
        printf("invalid param");
        return SOFTBUS_INVALID_PARAM;
    }

    uint32_t maxLen;

    //????
    if (SoftBusGetConfig(SOFTBUS_INT_MAX_MESSAGE_LENGTH,(unsigned char *)&maxLen, sizeof(maxLen)) != SOFTBUS_OK) {
        return SOFTBUS_GET_CONFIG_VAL_ERR;
    }

    if(len > maxLen) {
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t channelId = INVALID_CHANNEL_ID;
    int32_t type = CHANNEL_TYPE_BUTT;
    bool isEnable = false;

    if (ClientGetChannelBySessionId(sessionId, &channelId, &type,&isEnable) != SOFTBUS_OK)
    {
        return  SOFTBUS_TRANS_INVALID_SEESION_ID;
    }
    
    if (isEnable != true)
    {
        return SOFTBUS_TRANS_SESSION_OPENING;
    }
    
    //???
    return ClientTransChannelSendMessage(channelId, type, data,len);
 
}


//stream, file ....
