#include "include/soft_control.h"
#include "soft_control.c"


int32_t SoftControlInit()
{
    int32_t ret = CreateSoftBusController();
    return ret;   
}


int32_t SoftControlDeinit(int adapterId)
{
    int32_t ret = DestroySoftBusController(adapterId);
    return ret;
}


int32_t SoftControlAllDeinit()
{
    int32_t ret = DestroyAllSoftBusController();
    return ret;
}


int32_t SoftArbiterInit()
{
    int32_t ret = InitializeBusArbiters();
    return ret;
}

// int32_t SoftArbiterDeInit(int arbiterId)
// {

// }