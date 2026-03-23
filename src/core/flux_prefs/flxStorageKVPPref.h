
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
/*
 * flxStorageKVPPref.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#include "flxKVPStoreDevice.h"
#include "flxKVPStorePrefs.h"
#include "flxStorage.h"
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library
class flxStorageKVPPref;

class flxStorageKVPBlock : public flxStorageBlock
{

  public:
    flxStorageKVPBlock() : _prefs{nullptr}, _readOnly{false} {};

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
    friend flxStorageKVPPref;

    flxKVPStorePrefs *_prefs;
    bool _readOnly;

    void setPrefs(flxKVPStorePrefs *pPrefs)
    {
        _prefs = pPrefs;
    }
};

//------------------------------------------------------------------------------
// flxStorageKVPPref
//
// Interface for a storage system to persist state of a system

class flxStorageKVPPref : public flxStorage
{

  public:
    flxStorageKVPPref();

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
        // commit any changes
        _prefs.commit();
        _readOnly = false;
    }

    // public methods to manage a block
    flxStorageKVPBlock *beginBlock(const char *tag);

    flxStorageKVPBlock *getBlock(const char *tag);
    void endBlock(flxStorageBlock *);

    void resetStorage();

    void setStorageDevice(flxKVPStoreDevice *pDevice)
    {
        _prefs.setStorageDevice(pDevice);
    }

  private:
    // The block used to interface with the system
    flxStorageKVPBlock _theBlock;
    flxKVPStorePrefs _prefs;

    bool _readOnly;
};
