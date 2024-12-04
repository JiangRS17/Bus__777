#ifndef SOFTBUS_CONN_MANAGER_H
#define SOFTBUS_CONN_MANAGER_H

#include <stdint.h>

#define IP_LEN 46

//定义不同的软总结连接方式和模块
typedef enum {
    MODULE_MESSAGE_SERVICE = 8,
    MODULE_AUTH_CHANNEL = 9,
    MODULE_CONNECTION = 10,
} ConnModule;

//建立连接回调函数，断开连接和接收数据回调函数。
typedef struct {
    // void (*OnConnected)(uint32_t connectionId, const ConnectionInfo *info);
    // void (*OnDisConnected)(uint32_t connectionId, const ConnectionInfo *info);
    //建立连接
    void (*OnConnected)(uint32_t connectionId);
    //断开连接
    void (*OnDisConnected)(uint32_t connectionId);
    //接收数据
    void (*OnDataReceived)(uint32_t connectionId, ConnModule moduleId, int64_t seq, char *data, int32_t len);
} ConnectCallback;



typedef struct {
    void (*OnConnectSuccessed)(uint32_t requestId, uint32_t connnectionId);
    void (*OnConnectFailed)(uint32_t requestId, int32_t reason);
} ConnectResult;

typedef struct {
    // ConnectionType type;
    union 
    {
        struct IpListenerInfo
        {
            char ip[IP_LEN];
            int32_t port;
            int32_t modleId;
        }ipListenerInfo;
    } info;  
}LocalListenerInfo;

typedef struct 
{
    //connect the service
    int32_t (*ConnectDevice)(uint32_t requestId, const ConnectResult *result);

    //send data to peer
    int32_t (*PostBytes)(uint32_t connectionId, const char *data, int32_t len, int32_t pid,int32_t flag);

    //disconnect
    int32_t (*DisconnectDevice)(uint32_t connectionId);

    //disconnect all conecte devices
    // int32_t (*DisconnectDeviceNow)(const ConnectOption *option);

    //get the connection id
    int32_t (*GetConnectionInfo)(uint32_t connectionId);

    //启动本地监控服务，监听连接事件
    int32_t (*StartLocalListening)(const LocalListenerInfo *info);

    //关闭本地监控服务，停止监听连接事件
    int32_t (*StopLocalListening)(const LocalListenerInfo *info);

} ConnectFuncInterface;





#endif