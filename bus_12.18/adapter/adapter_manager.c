#include "include/soft_adapter.h"
// #include "soft_adapter.c"

int32_t Soft_Adapter_Init()
{
    int adapterId = globalAdapterCount;

    if (globalAdapterCount > SOFTBUS_MAX_LISTS)
    {
        printf("exceed limit");
        return SOFTBUS_ERR;
    }

    SoftBusAdapter *ret = CreateSoftBusAdapter(adapterId);
    if (ret == NULL)
    {
        printf("create sofebus adapter fail!");
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
}

int32_t Soft_Adapter_DeInit(int adpterId)
{
    int32_t ret = DestroySoftBusAdapter(adpterId);
    if (ret == SOFTBUS_ERR)
    {
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
    
}

void Soft_Adapter_AllDeInit()
{
    int32_t ret = DestroyAllSoftBusAdapter();
    printf("All SoftBusAdapters destroyed successfully.\n");
}