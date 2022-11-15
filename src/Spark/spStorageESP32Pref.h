/*
 * spStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#ifdef ESP32

#include <Preferences.h>
#include <cstdint>
#include <math.h>
#include <stddef.h>

#include "spStorage.h"
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library
class spStorageESP32Pref;

class spStorageESP32Block : public spStorageBlock {

public:
    spStorageESP32Block(): _prefs { nullptr }, _readOnly{false} {};

    bool writeBool(const char* tag, bool data);
    bool writeInt8(const char* tag, int8_t data);
    bool writeInt16(const char* tag, int16_t data);
    bool writeInt32(const char* tag, int32_t data);
    bool writeUInt8(const char* tag, uint8_t data);
    bool writeUInt16(const char* tag, uint16_t data);
    bool writeUInt32(const char* tag, uint32_t data);
    bool writeFloat(const char* tag, float data);
    bool writeDouble(const char* tag, double data);
    bool writeString(const char* tag, const char* data);

    bool readBool(const char* tag, bool& value, bool defaultValue = false);
    bool readInt8(const char* tag, int8_t& value, int8_t defaultValue = 0);
    bool readInt16(const char* tag, int16_t& value, int16_t defaultValue = 0);
    bool readInt32(const char* tag, int32_t& value, int32_t defaultValue = 0);
    bool readUInt8(const char* tag, uint8_t& value, uint8_t defaultValue = 0);
    bool readUInt16(const char* tag, uint16_t& value, uint16_t defaultValue = 0);
    bool readUInt32(const char* tag, uint32_t& value, uint32_t defaultValue = 0);
    bool readFloat(const char* tag, float& value, float defaultValue = NAN);
    bool readDouble(const char* tag, double& value, double defaultValue = NAN);
    size_t readString(const char* tag, char* data, size_t len);

    bool valueExists(const char* tag);

private:
    friend spStorageESP32Pref;

    Preferences* _prefs;
    bool         _readOnly;

    void setPrefs(Preferences* pPrefs)
    {
        _prefs = pPrefs;
    }

    void setReadOnly(bool readonly)
    {
        _readOnly=readonly;
    }
};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorageESP32Pref : public spStorage {

public:
    spStorageESP32Pref();

    // add begin, end stubs - the Esp32 prefs system doesn't required trasaction brackets
    bool begin(bool readonly=false)
    {
        _readOnly=readonly;
        return true;
    }
    void end(void)
    {
        _readOnly=false;
    }

    // public methods to manage a block
    spStorageESP32Block* beginBlock(const char* tag);

    spStorageESP32Block* getBlock(const char* tag);
    void endBlock(spStorageBlock*);

    void resetStorage();

private:
    // The block used to interface with the system
    spStorageESP32Block _theBlock;
    Preferences _prefs;

    bool _readOnly;
};

#endif