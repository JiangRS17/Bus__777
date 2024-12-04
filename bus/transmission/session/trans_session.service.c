#include "error.h"
#include "session.h"
#include "softbus.c"

#include<stdio.h>

static bool g_transSessionInitFlag = false;

int TransServerInit(void)
{
    if (g_transSessionInitFlag)
    {
        return SOFTBUS_OK;
    }

    if(TransSessionMgrInit() != SOFTBUS_OK) {
        printf("mgr init failed");
        return SOFTBUS_ERR;
    }
    if(TransChannelInit() != SOFTBUS_OK) {
        printf("channel init failed");
        return SOFTBUS_ERR;
    }

    //Qos

    g_transSessionInitFlag = true;
    return SOFTBUS_OK;
}

void TransServerDeinit(void)
{
    if (g_transSessionInitFlag == false)
    {
        return;
    }

    TransSessionMgrDeinit();
    TransChannelDeinit();
    g_transSessionInitFlag = false;
    
}

void TransServerDeathCallback(const char *pkgName)
{
    TransDelItemByPackageName(pkgName);
    TransChannelDeathCallback(pkgName);
}


