/*
 * flxStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#include "flxStorage.h"
#include "flxFS.h"

#include <Arduino.h>
#include <ArduinoJson.h>
//------------------------------------------------------------------------------
// Store prefs to a JSON file 

class flxStorageJSONPref;

class flxStorageJSONBlock : public flxStorageBlock {

public:
    flxStorageJSONBlock(): _readOnly{false}{}



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

    bool readBool(const char* tag, bool& value);
    bool readInt8(const char* tag, int8_t& value);
    bool readInt16(const char* tag, int16_t& value);
    bool readInt32(const char* tag, int32_t& value);
    bool readUInt8(const char* tag, uint8_t& value);
    bool readUInt16(const char* tag, uint16_t& value);
    bool readUInt32(const char* tag, uint32_t& value);
    bool readFloat(const char* tag, float& value);
    bool readDouble(const char* tag, double& value);
    size_t readString(const char* tag, char* data, size_t len);

    bool valueExists(const char* tag);

    flxStorage::flxStorageKind_t kind(void)
    {
        return flxStorage::flxStorageKindExternal;
    }
private:
    friend flxStorageJSONPref;

    JsonObject _jSection;

    bool _readOnly;

    void setObject(JsonObject &jsonSection)
    {
        _jSection = jsonSection;
    }

    void setReadOnly(bool readonly)
    {
        _readOnly=readonly;
    }
};

//------------------------------------------------------------------------------
// flxStorage
//
// Interface for a storage system to persist state of a system

class flxStorageJSONPref : public flxStorage {

public:
    flxStorageJSONPref() : _pDocument{nullptr}, _readOnly{false}, _fileSystem{nullptr}, _filename{""}
    {
        setName("JSON File", "Device setting storage using a JSON File");
    }

    flxStorageKind_t kind(void)
    {
        return flxStorage::flxStorageKindExternal;
    }
    // add begin, end stubs - the Esp32 prefs system doesn't required transaction brackets
    bool begin(bool readonly=false);

    void end(void);
    
    // public methods to manage a block
    flxStorageJSONBlock* beginBlock(const char* tag);

    flxStorageJSONBlock* getBlock(const char* tag);
    void endBlock(flxStorageBlock*);

    void resetStorage();

    void setFileSystem(flxIFileSystem *);
    void setFilename(std::string &name);
    void setFilename(const char *name)
    {
        std::string strName = name;
        setFilename(strName);
    }


private:

    void checkName();
    // The block used to interface with the system
    flxStorageJSONBlock _theBlock;

    // Pointer to the json document 

    DynamicJsonDocument  * _pDocument;

    bool _readOnly;

    flxIFileSystem * _fileSystem;
    std::string _filename;
};
