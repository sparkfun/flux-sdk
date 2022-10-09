/*
 * spStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#ifdef ESP32

#include <cstdint>
#include <stddef.h>
#include <math.h>
#include <Preferences.h>

//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library
class spStorageESP32Pref;

class spStorageESP32Block : public spStorageBlock2
{

  public:
    spStorageESP32Block() : _prefs{nullptr} {};

    bool writeBool(const char *tag, bool data);
    bool writeInt8(const char *tag, int8_t data);
    bool writeInt32(const char *tag, int32_t data);
    bool writeUInt8(const char *tag, uint8_t data);
    bool writeUInt32(const char *tag, uint32_t data);
    bool writeFloat(const char *tag, float data);
    bool writeDouble(const char *tag, double data);
    bool writeString(const char *tag, char *data);

    bool readBool(const char *tag, bool defaultValue = false );
    int8_t readInt8(const char *tag, int8_t defaultValue = 0 );
    int32_t readInt32(const char *tag, int32_t defaultValue = 0 );
    uint8_t readUInt8(const char *tag, uint8_t defaultValue = 0 );
    uint32_t readUInt32(const char *tag, uint32_t defaultValue = 0 );
    float readFloat(const char *tag, float defaultValue = NAN );
    double readDouble(const char *tag, double defaultValue = NAN) ;
    size_t readString(const char *tag, char *data, size_t len);

  private:
    friend spStorageESP32Pref;

    Preferences * _prefs;

    void setPrefs(Preferences *pPrefs)
    {
        _prefs = pPrefs;
    }
};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorageESP32Pref : public spStorage2
{

  public:
    spStorageESP32Pref();
    // public methods to manage a block
    spStorageESP32Block *beginBlock(const char *tag);

    spStorageESP32Block *getBlock(const char *tag);
    void endBlock(spStorageBlock2 *);

    void resetStorage();

private:
    // The block used to interface with the system
    spStorageESP32Block _theBlock;
    Preferences _prefs;
};

#endif