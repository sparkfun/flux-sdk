/*
 * spStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#include "spStorage.h"
#include "spFS.h"

#include <Arduino.h>
#include <ArduinoJson.h>
//------------------------------------------------------------------------------
// Store prefs to a JSON file 

class spStorageJSONPref;

class spStorageJSONBlock : public spStorageBlock {

public:
    spStorageJSONBlock(): _readOnly{false}{}



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

    spStorage::spStorageKind_t kind(void)
    {
        return spStorage::spStorageKindExternal;
    }
private:
    friend spStorageJSONPref;

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
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorageJSONPref : public spStorage {

public:
    spStorageJSONPref() : _pDocument{nullptr}, _readOnly{false}, _fileSystem{nullptr}, _filename{""}
    {
        setName("JSON File", "Device setting storage using a JSON File");
    }

    spStorageKind_t kind(void)
    {
        return spStorage::spStorageKindExternal;
    }
    // add begin, end stubs - the Esp32 prefs system doesn't required transaction brackets
    bool begin(bool readonly=false);

    void end(void);
    
    // public methods to manage a block
    spStorageJSONBlock* beginBlock(const char* tag);

    spStorageJSONBlock* getBlock(const char* tag);
    void endBlock(spStorageBlock*);

    void resetStorage();

    void setFileSystem(spIFileSystem *);
    void setFilename(std::string &name);
    void setFilename(const char *name)
    {
        std::string strName = name;
        setFilename(strName);
    }


private:

    void checkName();
    // The block used to interface with the system
    spStorageJSONBlock _theBlock;

    // Pointer to the json document 

    DynamicJsonDocument  * _pDocument;

    bool _readOnly;

    spIFileSystem * _fileSystem;
    std::string _filename;
};

