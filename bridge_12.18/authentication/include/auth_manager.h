#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "../../manager/include/bridge_manager.h"

#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>



extern DevicePrivilege currentPrivilege;  // 假设当前权限为 LOW


extern int32_t CheckPermission(const BridgeInfo *info_1);

#endif
