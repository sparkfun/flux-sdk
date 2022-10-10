/*
#
 */

#ifdef ESP32

#include "spStorageESP32Pref.h"
#include "spCoreLog.h"
#include "spUtils.h"

#include <nvs_flash.h>

#define kESP32HashTagSize 16

#define kESP32MinTagLen 4

// Storage system that leverages the ESP32 Preferences Arduino library
//
// Note: the Preferences library wants tags >= 15 chars. Do do this, we has our
// object names and print the names has (a number) to a string.
// handy helper
static bool tag_is_valid(const char *tag)
{
    if (!tag || strlen(tag) <= kESP32MinTagLen)
    {
        spLog_E("ESP32  Storage - invalid tag length - minimum is %d: %s\n\r", kESP32MinTagLen, !tag ? "NULL" : tag);
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library

// Write out a bool value
bool spStorageESP32Block::writeBool(const char *tag, bool value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putBool(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out an int8 value

bool spStorageESP32Block::writeInt8(const char *tag, int8_t value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putChar(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out a int value

bool spStorageESP32Block::writeInt32(const char *tag, int32_t value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Unsigned int8  - aka uchar

bool spStorageESP32Block::writeUInt8(const char *tag, uint8_t value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putUChar(szHash, value) > 0);
}
//------------------------------------------------------------------------
bool spStorageESP32Block::writeUInt32(const char *tag, uint32_t value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putUInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out a float

bool spStorageESP32Block::writeFloat(const char *tag, float value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putFloat(szHash, value) > 0);
}
//------------------------------------------------------------------------
// double

bool spStorageESP32Block::writeDouble(const char *tag, double value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putDouble(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Write out a c string
bool spStorageESP32Block::writeString(const char *tag, const char *value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize] = {0};

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putString(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Read value section
//------------------------------------------------------------------------

bool spStorageESP32Block::readBool(const char *tag, bool &value, bool defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getBool(szHash, defaultValue);

    return true;
}
//------------------------------------------------------------------------
bool spStorageESP32Block::readInt8(const char *tag, int8_t &value, int8_t defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getChar(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
bool spStorageESP32Block::readInt32(const char *tag, int32_t &value, int32_t defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getInt(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
bool spStorageESP32Block::readUInt8(const char *tag, uint8_t &value, uint8_t defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getUChar(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
bool spStorageESP32Block::readUInt32(const char *tag, uint32_t &value, uint32_t defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getUInt(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
bool spStorageESP32Block::readFloat(const char *tag, float &value, float defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getFloat(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
bool spStorageESP32Block::readDouble(const char *tag, double &value, double defaultValue)
{
    if (!tag_is_valid(tag) || !_prefs)
        return defaultValue;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return defaultValue;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getDouble(szHash, defaultValue);

    return true;
}

//------------------------------------------------------------------------
size_t spStorageESP32Block::readString(const char *tag, char *data, size_t len)
{
    if (!tag_is_valid(tag) || !_prefs)
        return 0;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return 0;

    if (!_prefs->isKey(szHash))
        return false;

    return _prefs->getString(szHash, data, len);
}

//------------------------------------------------------------------------------
bool spStorageESP32Block::valueExists(const char *tag)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kESP32HashTagSize];

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return _prefs->isKey(tag);
}
//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

spStorageESP32Pref::spStorageESP32Pref()
{
    _theBlock.setPrefs(&_prefs);
}

// public methods to manage a block
spStorageESP32Block *spStorageESP32Pref::beginBlock(const char *tag)
{
    if (!tag_is_valid(tag))
        return nullptr;

    char szHash[kESP32HashTagSize] = {0};

    if (!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return nullptr;

    if (!_prefs.begin(szHash, false))
    {
        spLog_E("Error creating settings storage");
        return nullptr;
    }

    return &_theBlock;
}

spStorageESP32Block *spStorageESP32Pref::getBlock(const char *tag)
{

    return beginBlock(tag);
}
void spStorageESP32Pref::endBlock(spStorageBlock2 *)
{
    _prefs.end();
}

void spStorageESP32Pref::resetStorage()
{
    // call low level ESP IDF functions to do this - as recommended by ESP32 docs ...
    nvs_flash_erase();
    nvs_flash_init();
}

#endif