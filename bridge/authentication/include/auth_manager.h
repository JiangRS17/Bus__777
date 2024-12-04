#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "bridge_manager.h"

DevicePrivilege currentPrivilege = LOW;  // 假设当前权限为 LOW


int32_t CheckPermission(const BridgeInfo *info_1);

#endif
