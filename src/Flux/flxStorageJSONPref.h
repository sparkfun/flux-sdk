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

#include "flxFS.h"
#include "flxStorage.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#define kDefaultJsonDocumentSize 6400
//------------------------------------------------------------------------------
// Store prefs to a JSON file

class flxStorageJSONPref;

class flxStorageJSONBlock : public flxStorageBlock
{

  public:
    flxStorageJSONBlock() : _readOnly{false}
    {
    }

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
        return flxStorage::flxStorageKindExternal;
    }

    void setReadOnly(bool readonly)
    {
        _readOnly = readonly;
    }
    
  private:
    friend flxStorageJSONPref;

    JsonObject _jSection;

    bool _readOnly;

    void setObject(JsonObject &jsonSection)
    {
        _jSection = jsonSection;
    }

    
};

//------------------------------------------------------------------------------
// flxStorage
//
// Interface for a storage system to persist state of a system

class flxStorageJSONPref : public flxStorage
{

  public:
    flxStorageJSONPref()
        : _pDocument{nullptr}, _readOnly{false}, _fileSystem{nullptr}, _filename{""},
          _jsonDocSize{kDefaultJsonDocumentSize}
    {
        setName("JSON File", "Device setting storage using a JSON File");
    }

    flxStorageKind_t kind(void)
    {
        return flxStorage::flxStorageKindExternal;
    }
    // add begin, end stubs - the Esp32 prefs system doesn't required transaction brackets
    bool begin(bool readonly = false);

    void end(void);

    // public methods to manage a block
    flxStorageJSONBlock *beginBlock(const char *tag);

    flxStorageJSONBlock *getBlock(const char *tag);
    void endBlock(flxStorageBlock *);

    void resetStorage();

    void setFileSystem(flxIFileSystem *);
    void setFilename(std::string &name);
    void setFilename(const char *name)
    {
        std::string strName = name;
        setFilename(strName);
    }

    void setBufferSize(size_t new_size)
    {
        // need a sane value
        if (new_size > 100)
            _jsonDocSize = new_size;
    }

    size_t bufferSize(void)
    {
        return _jsonDocSize;
    }

  private:
    void checkName();
    // The block used to interface with the system
    flxStorageJSONBlock _theBlock;

    // Pointer to the json document

    DynamicJsonDocument *_pDocument;

    bool _readOnly;

    flxIFileSystem *_fileSystem;
    std::string _filename;

    size_t _jsonDocSize;
};
