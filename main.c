#include "bus/bus/include/softbus_new.h"
#include "bus/include/softbus_error.h"
#include "bridge/manager/include/bridge_manager.h"
#include "bus/controller/include/soft_control.h"
#include "bus/adapter/include/soft_adapter.h"
#include "bus/controller/include/soft_arbiter.h"
#include "bridge/manager/bridge_manager.c"
#include "bus/bus/softbus_manager.c"
// #include "bus/controller/control_manger.c"
// #include "bridge/manager/bridge_conn.c"


#include <string.h>

//需求解析 Agent
//假设这里需要3条软总线(busneed = 3)

// Init 
int32_t CreateSoftBusArch (int busneed)
{
    // int busneed = 3;
    for (int i = 0; i < busneed; i++)
    {
        //创建软总线
        int32_t soft_ret = SoftBusMgrInit();    
        if (!soft_ret)
        {
            printf("softbus create error!");
            return SOFTBUS_ERR;
        }

        //连接软总线
        int32_t bridge_ret = BridgeMgrInit();
        if (!bridge_ret)
        {
             printf("bridge create error!");
            return SOFTBUS_ERR;
        }
        
        // 控制器 
        int32_t  controller_ret = SoftControlInit();
        if (!controller_ret)
        {
             printf("softbus controller create error!");
            return SOFTBUS_ERR;
        }
        
        // 适配器 
        int32_t  adapter_ret = Soft_Adapter_Init();
        if (!adapter_ret)
        {
             printf("softbus controller create error!");
            return SOFTBUS_ERR;
        }

        // 仲裁器
        int32_t  arbiter_ret = SoftArbiterInit();
        if (!arbiter_ret)
        {
             printf("softbus controller create error!");
            return SOFTBUS_ERR;
        }
        
    }
}

// publishInfo create 需求 build
int32_t InitializePublishInfoFromConfig(const char *configFilePath)
{
    //根据外部输入（如配置文件、用户命令等）解析每条软总线需要的组件类型和数量。
    //example:
    // publishId=0,type=TYPE_CPU,memory=2048,freq=HIGH,busIndex=0
    // publishId=1,type=TYPE_NPU,memory=1024,freq=MEDIUM,busIndex=1
    // read config file path
    FILE *file = fopen(configFilePath,"r");
    if (file == NULL)
    {
        printf("Error: Could not open configuration file %s\n", configFilePath);
        return SOFTBUS_ERR; 
    }
    
    char line[256];

    // 创建 PublishInfo：根据解析结果，为每条软总线生成对应数量的 PublishInfo 节点并初始化。
    // 关联到 SoftBusQueues：将这些 PublishInfo 节点与软总线队列进行关联。
    while (fgets(line, sizeof(line),file))
    {
        // 去除换行符
        line[strcspn(line, "\n")] = '\0';

        // 临时变量用于解析
        char type[32];
        char freq[16];

        int busIndex;

        PublishInfo *publishInfo = PublishMgrInit();
        if (publishInfo == NULL)
        {
            printf("Error: Failed to create PublishInfo for publishId %d\n", publishInfo->publishId);
            continue;
        }
        
        // 将 PublishInfo 添加到指定的总线队列
        int ret = AddComponentToQueue(busIndex,publishInfo);
        if (ret == SOFTBUS_ERR)
        {
            printf("Error: Failed to add PublishInfo %d to busIndex %d\n", publishInfo->publishId, busIndex);
            free(publishInfo);
        } else{
            printf("PublishInfo %d successfully added to busIndex %d\n", publishInfo->publishId, busIndex);
        }
        
    }
    
    fclose(file);
}


int main()
{
    // 需求解析
    int busneed = 3;

    // 初始化 Softbus & Bridge
    int32_t ret = CreateSoftBusArch(busneed);
    if (ret == SOFTBUS_ERR)
    {
        printf("create arch error!");
        return 0;
    } else{
        printf("create arch success!");
    }

    // 初始化 PublishInfo
    char *configFilePathe = "config.txt";
    int32_t info_ret = InitializePublishInfoFromConfig(configFilePathe);
    if (info_ret == SOFTBUS_ERR)
    {
        printf("create publishInfo error!");
        return 0;
    } else{
        printf("create publishInfo success!");
    }
    
    


    return 0;
}
