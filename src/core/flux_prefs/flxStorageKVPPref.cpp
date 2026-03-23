/*
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
#include "flxStorageKVPPref.h"
#include "flxCoreLog.h"
#include "flxUtils.h"

#define kHashTagSize 16

#define kMinTagLen 3

// Note: the Preferences library wants tags >= 15 chars. Do do this, we has our
// object names and print the names has (a number) to a string.
// handy helper
static bool tag_is_valid(const char *tag)
{
    if (!tag || strlen(tag) < kMinTagLen)
    {
        flxLog_E("Preference  Storage - invalid tag length - minimum is %d: %s\n\r", kMinTagLen, !tag ? "NULL" : tag);
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library

// Write out a bool value
bool flxStorageKVPBlock::writeBool(const char *tag, bool value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putBool(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out an int8 value

bool flxStorageKVPBlock::writeInt8(const char *tag, int8_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putChar(szHash, value) > 0);
}

//------------------------------------------------------------------------
// write out an int16 value

bool flxStorageKVPBlock::writeInt16(const char *tag, int16_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putShort(szHash, value) > 0);
}

//------------------------------------------------------------------------
// write out a int value

bool flxStorageKVPBlock::writeInt32(const char *tag, int32_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Unsigned int8  - aka uchar

bool flxStorageKVPBlock::writeUInt8(const char *tag, uint8_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putUChar(szHash, value) > 0);
}

//------------------------------------------------------------------------
// Unsigned int16  - aka ushort

bool flxStorageKVPBlock::writeUInt16(const char *tag, uint16_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putUShort(szHash, value) > 0);
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::writeUInt32(const char *tag, uint32_t value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putUInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out a float

bool flxStorageKVPBlock::writeFloat(const char *tag, float value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putFloat(szHash, value) > 0);
}
//------------------------------------------------------------------------
// double

bool flxStorageKVPBlock::writeDouble(const char *tag, double value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putDouble(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Write out a c string
bool flxStorageKVPBlock::writeString(const char *tag, const char *value)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    // the value length is 0, just return true. Otherwise the esp pref system
    // returns an error.

    if (!value || strlen(value) == 0)
        return true;

    char szHash[kHashTagSize] = {0};

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putString(szHash, value) > 0);
}

//------------------------------------------------------------------------
// Write out an array of bytes

bool flxStorageKVPBlock::writeBytes(const char *tag, const uint8_t *value, size_t len)
{
    if (!tag_is_valid(tag) || !_prefs || _readOnly)
        return false;

    // the value length is 0, just return true. Otherwise the esp pref system
    // returns an error.

    if (!value || len == 0)
        return true;

    char szHash[kHashTagSize] = {0};

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return (_prefs->putBytes(szHash, (void *)value, len) > 0);
}
//------------------------------------------------------------------------
// Read value section
//------------------------------------------------------------------------

bool flxStorageKVPBlock::readBool(const char *tag, bool &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getBool(szHash);

    return true;
}
//------------------------------------------------------------------------
bool flxStorageKVPBlock::readInt8(const char *tag, int8_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getChar(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readInt16(const char *tag, int16_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getShort(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readInt32(const char *tag, int32_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getInt(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readUInt8(const char *tag, uint8_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getUChar(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readUInt16(const char *tag, uint16_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getUShort(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readUInt32(const char *tag, uint32_t &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getUInt(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readFloat(const char *tag, float &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getFloat(szHash);

    return true;
}

//------------------------------------------------------------------------
bool flxStorageKVPBlock::readDouble(const char *tag, double &value)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    if (!_prefs->isKey(szHash))
        return false;

    value = _prefs->getDouble(szHash);

    return true;
}

//------------------------------------------------------------------------
size_t flxStorageKVPBlock::readString(const char *tag, char *data, size_t len)
{
    if (!tag_is_valid(tag) || !_prefs)
        return 0;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return 0;

    if (!_prefs->isKey(szHash))
        return 0;

    return _prefs->getString(szHash, data, len);
}
//------------------------------------------------------------------------------
size_t flxStorageKVPBlock::getStringLength(const char *tag)
{
    if (!tag_is_valid(tag) || !_prefs)
        return 0;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return 0;

    if (!_prefs->isKey(szHash))
        return 0;

    String tmp = _prefs->getString(szHash);

    return tmp.length();
}
//------------------------------------------------------------------------
size_t flxStorageKVPBlock::readBytes(const char *tag, uint8_t *data, size_t len)
{
    if (!tag_is_valid(tag) || !_prefs)
        return 0;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return 0;

    if (!_prefs->isKey(szHash))
        return 0;

    size_t nBytes = _prefs->getBytesLength(szHash);

    // room in buffer?
    if (nBytes > len)
        return 0;

    return _prefs->getBytes(szHash, data, len);
}
//------------------------------------------------------------------------------
size_t flxStorageKVPBlock::getBytesLength(const char *tag)
{
    if (!tag_is_valid(tag) || !_prefs)
        return 0;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return 0;

    if (!_prefs->isKey(szHash))
        return 0;

    return _prefs->getBytesLength(szHash);
}
//------------------------------------------------------------------------------
bool flxStorageKVPBlock::valueExists(const char *tag)
{
    if (!tag_is_valid(tag) || !_prefs)
        return false;

    char szHash[kHashTagSize];

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return false;

    return _prefs->isKey(szHash);
}
//------------------------------------------------------------------------------
// flxStorageKVPPref
//------------------------------------------------------------------------------

//
// Interface for a storage system to persist state of a system

flxStorageKVPPref::flxStorageKVPPref() : _readOnly{false}
{
    _theBlock.setPrefs(&_prefs);
    setName("Preferences", "Device setting storage ");

    // Setup the NVS partition - prefs storage - as encrypted

    // if (!setupNVSSecurePartition())
    //     flxLog_N(F(" Preference storage is not encrypted"));
    // else
    //     flxLog_I(F("On-board preference storage is encrypted"));
}

// public methods to manage a block
flxStorageKVPBlock *flxStorageKVPPref::beginBlock(const char *tag)
{
    if (!tag_is_valid(tag))
        return nullptr;

    char szHash[kHashTagSize] = {0};

    if (!flx_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
        return nullptr;

    if (!_prefs.begin(szHash, false))
    {
        flxLog_E("Error creating settings storage");
        return nullptr;
    }
    _theBlock.setReadOnly(_readOnly);

    return &_theBlock;
}

flxStorageKVPBlock *flxStorageKVPPref::getBlock(const char *tag)
{

    return beginBlock(tag);
}
void flxStorageKVPPref::endBlock(flxStorageBlock *)
{
    _prefs.end();
}

void flxStorageKVPPref::resetStorage()
{
    // Clear out the storage area / pages
    _prefs.reset();
    // call low level ESP IDF functions to do this - as recommended by ESP32 docs ...
    // nvs_flash_erase();
    // nvs_flash_init();
}
