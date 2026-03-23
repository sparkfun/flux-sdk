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

#include <hardware/watchdog.h>
#include <malloc.h>
#include <pico/unique_id.h>

// rpi version of our platform class

//---------------------------------------------------------------------------------
/// @brief Return a unique identifier for the device - a 12 char hex string
/// @return const char* - the unique identifier
///
const char *flxPlatform::unique_id(void)
{
    static char szDeviceID[13] = {0};
    if (szDeviceID[0] == 0)
    {
        pico_get_unique_board_id_string(szDeviceID, sizeof(szDeviceID));
    }
    return szDeviceID;
}

//---------------------------------------------------------------------------------
/// @brief Restart the device
///
void flxPlatform::restart_device(void)
{
    watchdog_reboot(0, 0, 0);
}

// memory things
uint32_t flxPlatform::heap_size(void)
{
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

// free heap
uint32_t flxPlatform::heap_free(void)
{
    struct mallinfo m = mallinfo();

    return heap_size() - m.uordblks;
}
