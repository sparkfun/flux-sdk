

/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

//----------------------------------------------------------
// A Key-Value-Pair Storage system
//
// This is a simple key-value-pair storage system that is designed to be used for persistent storage
// of settings/preferences.
//
// The system is designed to mimic the ESP32 Preferences library, heavily borrowed/copied the design
// pattern of that system.
//----------------------------------------------------------
#include "flxKVPStoreEntry.h"

#include "flxUtils.h"
#include <cstddef>

uint32_t flxKVPStoreEntry::calculateCRC32() const
{
    uint32_t result = 0xffffffff;

    const uint8_t *pData = reinterpret_cast<const uint8_t *>(this);

    result = flx_utils::calc_crc32(result, pData + offsetof(flxKVPStoreEntry, iNameSpace),
                                   offsetof(flxKVPStoreEntry, crc32) - offsetof(flxKVPStoreEntry, iNameSpace));

    result = flx_utils::calc_crc32(result, pData + offsetof(flxKVPStoreEntry, entryKey), sizeof(entryKey));

    result = flx_utils::calc_crc32(result, pData + offsetof(flxKVPStoreEntry, data), sizeof(data));

    return result;
}
uint32_t flxKVPStoreEntry::calculateCRC32(const uint8_t *pData, size_t size)

{
    return flx_utils::calc_crc32(0xffffffff, pData, size);
}