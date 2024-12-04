#include "../include/trans_manager.h"
#include "../bridge_error.h"
#include "../authentication/include/auth_manager.h"

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>



int32_t ReadBridgeDeviceTable(const BridgeInfo *info, int requestId) {

    int found = 0;  // 标记是否找到匹配的桥节点

    if (isbridge == false) {
        printf("Error: Input BridgeInfo is NULL\n");
        return BRIDGE_ERR;
    }

    
    for (int i = 0; i < bridgeCounter; i++)
    {
        if (globalBridegInfo[i].bridgeType == TYPE_1){
            if (globalBridegInfo[i].func_code.funId == requestId) //桥节点配置表中是否存在请求服务
            {

                found = 1;
                // 检查权限
                if (CheckPermission(globalBridegInfo[i].privilege)) {
                    globalBridegInfo[i].bridgeType = TYPE_0;
                    //将请求放到队列
                    // CreatePendingItem(channelId, seqNum);    
                } else {
                    printf("Permission check failed for Bridge Node ID %d\n", globalBridegInfo[i].bridgeId);
                }
                // int32_t channelId = info->busnum.secondary_bus_num;
                // int32_t seqNum = requestId;  //传递请求服务号
                // //将请求放到队列
                // CreatePendingItem(channelId, seqNum);
            }
        }
    }

    // 遍历结束，未找到匹配项

    // 返回结果
    if (found) {
        return BRIDGE_OK;  // 成功找到
    } else {
        printf("No matching bridge node found for the requested service ID %d\n", requestId);
        return BRIDGE_ERR;  // 没有找到
    }

    // if (info->bridgeType == TYPE_0)
    // {
    //     /* code */
    //     //handle
    //     // xxx
    //     // BridgeHandle();
    //     int32_t channelId = info->busnum.secondary_bus_num;
    //     int32_t seqNum = requestId;  //传递请求服务号
    //     //auth
    //     // CheckPermission();
    //     CreatePendingItem(channelId, seqNum);
    //     return BRIDGE_OK;
    // }

    // int num = info->busnum.secondary_bus_num;;
    // //look up bridge table
    // if(isbridge == true && num <= info->busnum.sub_bus_num)  
    // {
    //     /* code */
    //     for (int i = 0; i <= FUN_MAX_DEVICE_LD_LEN; i++)
    //     {
    //         if (info->func_code[i] == requestId)   //桥节点配置表中是否存在请求服务
    //         {
    //             //auth
    //             BridgeInfo *info_2 = globalBridegInfo[num++];
    //             CheckPermission(info,info_2);
    //             info->bridgeType = TYPE_0;
    //             //handle
    //             //
    //             int32_t channelId = info->busnum.secondary_bus_num;
    //             int32_t seqNum = requestId;  //传递请求服务号
    //             CreatePendingItem(channelId, seqNum);
    //             return BRIDGE_OK;
    //         }
    //     }

    //     num ++ ;
    // }
}
