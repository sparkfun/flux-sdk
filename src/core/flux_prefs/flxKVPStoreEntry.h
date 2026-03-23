
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
// Entry record for the storage data

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "flxCoreTypes.h"
#include "flxKVPStoreDefs.h"

// FPS - Flash Preference Storage
// Define an record/entry in the pref table

class flxKVPStoreEntry
{

  public:
    static constexpr uint32_t kEntrySize = 32;
    static constexpr uint32_t kEntryInvalid = 0xFFFF0000;

    union {
        struct
        {
            uint8_t iNameSpace;
            flxDataType_t dataType;
            uint8_t span;
            uint8_t fill;
            uint32_t crc32;
            char entryKey[kKVPMaxKeyNameLength];
            union {
                struct
                {
                    uint16_t dataSize;
                    uint16_t reserved;
                    uint32_t dataCRC32;
                } dataLength;
                uint8_t data[8];
            };
        };
        uint8_t record[32];
    };

    static constexpr uint16_t kMaxKeyLength = kKVPMaxKeyNameLength - 1;

    flxKVPStoreEntry() : span{1}
    {
    }

    flxKVPStoreEntry(uint8_t iNS, flxDataType_t dType, uint8_t span, const char *szKey)
        : iNameSpace{iNS}, dataType{dType}, span{span}
    {
        memset((void *)entryKey, 0xff, kKVPMaxKeyNameLength);
        memset((void *)data, 0xff, sizeof(data));

        if (szKey)
        {
            strncpy(entryKey, szKey, kKVPMaxKeyNameLength);
            entryKey[kMaxKeyLength] = '\0';
        }
        else
            entryKey[0] = '\0';
    }

    void getKey(char *szDest, size_t len)
    {
        strncpy(szDest, entryKey, std::min(len, kKVPMaxKeyNameLength));
        szDest[len - 1] = '\0'; // make sure we terminate the string
    }

    template <typename T> bool getValue(T &dest)
    {
        dest = *reinterpret_cast<T *>(data);

        return true;
    }

    uint32_t calculateCRC32() const;
    static uint32_t calculateCRC32(const uint8_t *data, size_t size);
};
