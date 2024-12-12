//session.h: 声明统一数据传输接口
/*
  该文件提供数据传输功能，包括创建和删除会话服务器、打开和关闭会话、接收数据以及查询基本会话信息。
  在发现多个附近的设备并联网后，这些接口可用于在设备间传输数据。
*/

#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#define MSG_FLAG_REQUEST 0
#define MSG_FLAG_REPLY 1


typedef struct {
    const char *sessionName;
    const char *perseesionName;
    const char *peerDeviceId;
    const char *groupId;
} SessionParam;

typedef struct {
    int32_t channelId;
    int32_t channelType;
    int32_t eventId;
    // int32_t tvCount;
} QosParam;

typedef struct 
{
    int32_t channelId;
    int32_t channelType;
} TransInfo;

typedef struct {
    TransInfo transInfo;
    int32_t ret;
} TransSerializer;




typedef enum {
    TYPE_MESSAGE = 1,
    TYPE_BYTES,
    TYPE_FILE,
    TYPE_STREAM,
} SessionType;

typedef struct {
    char *buf;
    int bufLen;
} StreamData;

typedef struct {
    int frameType;
    int64_t timeStamp;
    int seqNum;
    int seqSubNum;
    int level;
    int bitMap;
} StreamFrameInfo;

// 定义会话回调
// when a session is opened or closed, or there is data to process, the related callback is invoked.
typedef struct {
    // open session
    int (*OnSessionOpened)(int sessionId, int result);
    //close session
    void (*OnSessionClosed)(int sessionId);
    //notify that data is recevied
    void (*OnByteRecevied)(int sessionId, const void *data, unsigned int dataLen);
    //notift message is recevied
    void (*OnMessageRecevied)(int sessionId, const void *data, unsigned int dataLen);
    //stream
    void (*OnStreamReceived)(int sessionId, const StreamData *data, const StreamData *ext, const StreamFrameInfo *param);
} ISessionListener;

typedef struct {
    int (*OnReceiveFileStarted)(int sessionId, const char *files, int fileCnt);
    int (*OnReceiveFileProcess)(int sessionId, const char *firstFile, uint64_t bytesUpload, uint64_t bytesTotal);
    void (*OnReceiveFileFinished)(int sessionId, const char *files, int fileCnt);
    void (*OnFileTransError)(int sessionId);
} IFileReceiveListener;

typedef struct {
    int (*OnSendFileProcess)(int sessionId, uint64_t bytesUpload,uint64_t bytesTotal);
    int (*OnSendFileFinished)(int sessionId, const char *firstFile);
    void (*OnFileTransError)(int sessionId);
} IFileSendListener;

//创建一个会话基于模块名字和会话名字
int CreateSessionServer(const char *pkgName, const char *sessionName, const ISessionListener *listener);

//移除一个会话基于模块名字和会话名字
int RemoveSessionServer(const char *pkgName, const char *sessionName);

//打开一个会话请求（异步）
//OnSessionOpened 会通知用户会话是否成功打开
int OpenSession(const char *mySessionName, const char *peerSessionName);
// int OpenSession(const char *mySessionName, const char *peerSessionName, const char *peerNetworkId,
//     const char *groupId, const SessionAttribute* attr);

//close一个会话请求
void CloseSession(int sessionId);

//发送数据基于会话ID
int SendBytes(int sessionId, const void *data, unsigned int len);

//发送message基于会话ID
int SendMessage(int sessionId, const StreamData *data, const StreamData *ext, const StreamFrameInfo *param);

//根据会话ID获取本地设备注册的会话名称
int GetMySessionName(int sessionId, char *sessionName, unsigned int len);

//根据会话ID获取对端(peer)设备注册的会话名称
int GetPeerSessionName(int sessionId, char *sessionName, unsigned int len);

int GetPeerDeviceId(int sessionId, char *networkId, unsigned int len);

int GetSessionSide(int sessionId);

int SetFileReceiveListener(const char *pkgName, const char *sessionName,const IFileReceiveListener *recvListener, const char *rootDir);

int SetFileSendListener(const char *pkgName, const char *sessionName, const IFileReceiveListener *sendListener);

int SendFile(int sessionId, const char *sFileList[], const char *dFileList[], uint32_t fileCnt);


//session manager
typedef struct {
    ListNode node;
    // SoftBusSecType type;
    char pkgName[PKG_NAME_SIZE_MAX];
    char sessionName[SESSION_NAME_SIZE_MAX];
    int32_t uid;
    int32_t pid;
} SessionServer;


int32_t TransSessionMgrInit(void);

void TransSessionMgrDeinit(void);

bool TransSessionServerIsExist(const char *sessionName);

int32_t TransSessionServerAddItem(SessionServer *newNode);

int32_t TransSessionServerDelItem(const char *sessionName);

void TransDelItemByPackageName(const char *sessionName, char *pkgName, uint16_t len);

int32_t TransGetPkgNameBySessionName(const char *sessionName, char *pkgName, uint16_t len);

int32_t TransGetUidAndPid(const char *sessionName, int32_t *uid, int32_t *pid);

void TransOnLinkDown(const char *networkId, int32_t routeType);



//session service
int32_t TransCreatSessionServer(const char *pkgName, const char *sessionName, int32_t uid, int32_t pid);

int32_t TransRemoveSessionServer(const char *pkgName, const char *sessionName);

int32_t TransOpenSession(const SessionParam *param, TransInfo *info);

int TransServerInit(void);

void TransServerDeinit(void);

void TransServerDeathCallback(const char *pkgName);


#endif
