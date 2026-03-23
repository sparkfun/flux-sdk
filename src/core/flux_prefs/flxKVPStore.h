
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

#pragma once
// For the main class of the system

#include "flxCoreLog.h"
#include "flxKVPStoreDefs.h"
#include "flxKVPStoreDevice.h"
#include "flxKVPStorePage.h"
#include <bitset>
#include <vector>

class flxKVPStore
{
  public:
    static constexpr int16_t kNullPage = -1;

    flxKVPStore() : _storageDevice{nullptr}, _currPage{kNullPage}
    {
    }
    flxKVPError_t initialize(void);

    // get a storage handle/id - returns 0 on error
    uint8_t getNameSpace(const char *szNS);

    // set methods
    //-----------------------------------------------------------------------

    // Strings need their own method entries -- to catch the value type and get length correct
    flxKVPError_t setValue(uint8_t iNS, const char *szKey, const char *value)
    {
        return setValueString(iNS, szKey, value, strlen(value));
    }
    flxKVPError_t setValue(uint8_t iNS, const char *szKey, char *value)
    {
        return setValueString(iNS, szKey, (const char *)value, strlen(value));
    }
    template <typename T> flxKVPError_t setValue(uint8_t iNS, const char *szKey, T &value)
    {
        // flxLog_I("setValue: %s", szKey);
        return setValue(iNS, flxGetTypeOf(value), szKey, &value, sizeof(value));
    }

    flxKVPError_t setValue(uint8_t iNS, const char *szKey, const void *value, size_t valueSize)
    {
        return setValueString(iNS, szKey, (const char *)value, valueSize);
    }

    //-----------------------------------------------------------------------
    // get values

    template <typename T> flxKVPError_t getValue(uint8_t iNS, const char *szKey, T &value)
    {
        // flxLog_I("getValue: %s", szKey);
        return getValue(iNS, flxGetTypeOf(value), szKey, &value, sizeof(value));
    }

    flxKVPError_t getValue(uint8_t iNS, const char *szKey, char *value, size_t len)
    {
        return getValue(iNS, flxTypeString, szKey, (void *)value, len);
    }

    flxKVPError_t getValue(uint8_t iNS, const char *szKey, void *value, size_t len)
    {
        // flxLog_I("getValue: %s", szKey);
        return getValue(iNS, flxTypeString, szKey, (void *)value, len);
    }

    flxKVPError_t deleteValue(uint8_t iNS, const char *szKey);

    bool keyExists(uint8_t iNS, const char *szKey);

    void commit(void)
    {
        if (_storageDevice)
            _storageDevice->flush();
    }

    void reset(void);

    void setStorageDevice(flxKVPStoreDevice *device)
    {
        _storageDevice = device;
    }
    void setStorageDevice(flxKVPStoreDevice &device)
    {
        setStorageDevice(&device);
    }

  protected:
    flxKVPError_t getNameSpaceIndex(const char *szNS, uint8_t &outNSIndex);

  private:
    bool moveToFreePage(void);

    flxKVPError_t checkNameSpaces(void);

    // setValue
    flxKVPError_t setValue(uint8_t iNS, flxDataType_t dType, const char *szKey, const void *value, size_t valueSize);
    flxKVPError_t setValueString(uint8_t iNS, const char *szKey, const char *value, size_t valueSize);

    // read value
    flxKVPError_t getValue(uint8_t iNS, flxDataType_t dType, const char *szKey, void *value, size_t valueSize);

    struct KVPNameSpaceEntry
    {
        uint8_t index;
        char name[kKVPMaxKeyNameLength];
    };

    flxKVPStoreDevice *_storageDevice;

    int16_t _currPage;

    std::vector<flxKVPStorePage *> _pages;
    std::vector<KVPNameSpaceEntry *> _namespaces;
    std::bitset<256> _nsState;
};