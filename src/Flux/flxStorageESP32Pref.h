/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 * flxStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#ifdef ESP32

#include <Preferences.h>
#include <cstdint>
#include <math.h>
#include <stddef.h>

#include "flxStorage.h"
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library
class flxStorageESP32Pref;

class flxStorageESP32Block : public flxStorageBlock
{

  public:
    flxStorageESP32Block() : _prefs{nullptr}, _readOnly{false} {};

    bool writeBool(const char *tag, bool data);
    bool writeInt8(const char *tag, int8_t data);
    bool writeInt16(const char *tag, int16_t data);
    bool writeInt32(const char *tag, int32_t data);
    bool writeUInt8(const char *tag, uint8_t data);
    bool writeUInt16(const char *tag, uint16_t data);
    bool writeUInt32(const char *tag, uint32_t data);
    bool writeFloat(const char *tag, float data);
    bool writeDouble(const char *tag, double data);
    bool writeString(const char *tag, const char *data);
    bool writeBytes(const char *tag, const uint8_t *data, size_t len);

    bool readBool(const char *tag, bool &value);
    bool readInt8(const char *tag, int8_t &value);
    bool readInt16(const char *tag, int16_t &value);
    bool readInt32(const char *tag, int32_t &value);
    bool readUInt8(const char *tag, uint8_t &value);
    bool readUInt16(const char *tag, uint16_t &value);
    bool readUInt32(const char *tag, uint32_t &value);
    bool readFloat(const char *tag, float &value);
    bool readDouble(const char *tag, double &value);
    size_t readString(const char *tag, char *data, size_t len);
    size_t readBytes(const char *tag, uint8_t *data, size_t len);

    size_t getStringLength(const char *tag);
    size_t getBytesLength(const char *tag);

    bool valueExists(const char *tag);

    flxStorage::flxStorageKind_t kind(void)
    {
        return flxStorage::flxStorageKindInternal;
    }

    void setReadOnly(bool readonly)
    {
        _readOnly = readonly;
    }

  private:
    friend flxStorageESP32Pref;

    Preferences *_prefs;
    bool _readOnly;

    void setPrefs(Preferences *pPrefs)
    {
        _prefs = pPrefs;
    }
};

//------------------------------------------------------------------------------
// flxStorage
//
// Interface for a storage system to persist state of a system

class flxStorageESP32Pref : public flxStorage
{

  public:
    flxStorageESP32Pref();

    flxStorageKind_t kind(void)
    {
        return flxStorage::flxStorageKindInternal;
    }

    // add begin, end stubs - the Esp32 prefs system doesn't required transaction brackets
    bool begin(bool readonly = false)
    {
        _readOnly = readonly;
        return true;
    }
    void end(void)
    {
        _readOnly = false;
    }

    // public methods to manage a block
    flxStorageESP32Block *beginBlock(const char *tag);

    flxStorageESP32Block *getBlock(const char *tag);
    void endBlock(flxStorageBlock *);

    void resetStorage();

  private:
    // bool setupNVSSecurePartition(void);

    // The block used to interface with the system
    flxStorageESP32Block _theBlock;
    Preferences _prefs;

    bool _readOnly;
};

#endif