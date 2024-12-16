
#include "../manager/include/bridge_manager.h"
#include "include/auth_manager.h"

#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

int32_t CheckPermission(const BridgeInfo *info_1) {
    int bridgepriv_1 = info_1->privilege;

    if (bridgepriv_1 <= currentPrivilege)
    {
        return BRIDGE_OK;
    } else {
        return BRIDGE_ERR;
    }
}