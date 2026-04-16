/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxPlatform.h"
#include "flxCoreLog.h"
#include <Esp.h>

// esp version of our platform class

//---------------------------------------------------------------------------------
/// @brief Return a unique identifier for the device - a 12 char hex string
/// @return const char* - the unique identifier
///
const char *flxPlatform::unique_id(void)
{
    static char szDeviceID[13] = {0};
    if (szDeviceID[0] == 0)
    {
        uint64_t chipid = ESP.getEfuseMac();
        snprintf(szDeviceID, sizeof(szDeviceID), "%012llX", ESP.getEfuseMac());
    }
    return szDeviceID;
}

//---------------------------------------------------------------------------------
/// @brief Restart the device
///
void flxPlatform::restart_device(void)
{
    esp_restart();
}

// memory things
uint32_t flxPlatform::heap_size(void)
{
    // take into account PSRAM
    return ESP.getHeapSize() + ESP.getPsramSize();
}

// free heap
uint32_t flxPlatform::heap_free(void)
{
    // take into account PSRAM
    return ESP.getFreeHeap() + ESP.getFreePsram();
}
