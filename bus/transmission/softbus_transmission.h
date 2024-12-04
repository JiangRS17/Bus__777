
#ifndef SOFTBUS_TRANSMISSION_H
#define SOFTBUS_TRANSMISSION_H

#include <stdint.h>

typedef struct 
{
    int (*onChannelOpened)(int32_t channelId, const char *uuid, unsigned char isServer);;
    void (*onChannelOpenFailed)(int32_t channelId, const char **uuid);
    void (*onChannelClosed)(int32_t channelId);
    void (*onMessageReceived)(int32_t channelId, const char *data, uint32_t len);
} INetworkingListener;

int TransOpenNetWorkingChannel(const char *sessionName, const char *peerNetworkId);

int TransCloseNetWorkingChannel(int32_t channelId);

int TransSendNetworkingMessage(int32_t channelId, const char *data, uint32_t datalen, int32_t priority);

int TransRegisterNetworkingChannelListener(const INetworkingListener *listener);


//transmission pending
 enum {
    PENDING_TYPE_PROXY = 0,
    PENDING_TYPE_DIRECT,
    PENDING_TYPE_BUFF,
 };

 
 int32_t PendingInit(int type);
 void PendingDeinit(int type);
 int32_t ProcPendingPacket(int32_t channelId, int32_t seqNum, int type);
 int32_t SetPendingPacket(int32_t channelId, int32_t seqNum, int type);
 int32_t DelPendingPacket(int32_t channelId, int type);

typedef struct 
{
    int32_t (*GetPendingPacketChannleId)(int32_t channelId);
    
} PendingPacketInfo;



#endif