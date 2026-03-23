
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

// mimic the interface to the ESP32 Arduino preference library

#include "flxKVPStore.h"
#include <Arduino.h>

class flxKVPStorePrefs : public flxKVPStore
{
  public:
    flxKVPStorePrefs() : flxKVPStore(), _iNameSpace{kNoNameSpace} {};

    static constexpr const int32_t kNoNameSpace = -1;

    bool begin(const char *name, bool readOnly = false)
    {
        uint8_t tmp;
        if (getNameSpaceIndex(name, tmp) != kKVPErrorOK)
            return false;

        _iNameSpace = tmp;

        return true;
    }

    void end()
    {
        _iNameSpace = kNoNameSpace;
    }

    size_t putChar(const char *key, int8_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? 1 : 0;
    }

    size_t putUChar(const char *key, uint8_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? 1 : 0;
    }

    size_t putShort(const char *key, int16_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putUShort(const char *key, uint16_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putInt(const char *key, int32_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putUInt(const char *key, uint32_t value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putLong(const char *key, int32_t value)
    {
        return putInt(key, value);
    }
    size_t putULong(const char *key, uint32_t value)
    {
        return putUInt(key, value);
    }

    size_t putFloat(const char *key, float value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putDouble(const char *key, double value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? sizeof(value) : 0;
    }

    size_t putBool(const char *key, bool value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? 1 : 0;
    }

    size_t putString(const char *key, const char *value)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value) == kKVPErrorOK ? strlen(value) : 0;
    }

    size_t putBytes(const char *key, const void *value, size_t len)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return setValue(_iNameSpace, key, value, len) == kKVPErrorOK ? len : 0;
    }

    int8_t getChar(const char *key, int8_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        int8_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    uint8_t getUChar(const char *key, uint8_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        uint8_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    int16_t getShort(const char *key, int16_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        int16_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    uint16_t getUShort(const char *key, uint16_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        uint16_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    int32_t getLong(const char *key, int32_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        int32_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    uint32_t getULong(const char *key, uint32_t defaultValue = 0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        uint32_t value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    int32_t getInt(const char *key, int32_t defaultValue = 0)
    {
        return getLong(key, defaultValue);
    }

    uint32_t getUInt(const char *key, uint32_t defaultValue = 0)
    {
        return getULong(key, defaultValue);
    }

    float getFloat(const char *key, float defaultValue = 0.0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        float value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    double getDouble(const char *key, double defaultValue = 0.0)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        double value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    bool getBool(const char *key, bool defaultValue = false)
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        bool value;
        return getValue(_iNameSpace, key, value) == kKVPErrorOK ? value : defaultValue;
    }

    size_t getString(const char *key, char *data, size_t len)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return getValue(_iNameSpace, key, data, len) == kKVPErrorOK ? strlen(data) : 0;
    }

    String getString(const char *key, const String &defaultValue = "")
    {
        if (_iNameSpace == kNoNameSpace)
            return defaultValue;

        char data[512];
        return getValue(_iNameSpace, key, data, sizeof(data)) == kKVPErrorOK ? String(data) : defaultValue;
    }
    size_t getBytes(const char *key, void *data, size_t len)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        return getValue(_iNameSpace, key, data, len) == kKVPErrorOK ? len : 0;
    }

    size_t getBytesLength(const char *key)
    {
        if (_iNameSpace == kNoNameSpace)
            return 0;

        char data[512];
        return getValue(_iNameSpace, key, data, sizeof(data)) == kKVPErrorOK ? strlen(data) : 0;
    }

    bool isKey(const char *key)
    {
        if (_iNameSpace == kNoNameSpace)
            return false;

        return keyExists(_iNameSpace, key);
    }

  private:
    int32_t _iNameSpace;
};