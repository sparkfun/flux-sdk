/*
 * spStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#include <cstdint>
#include <stddef.h>
#include <math.h>

// Storage involves two interfaces
//
//  - spStorage       - The main interface into the storage system being used.
//  - spStorageBlock  - Interface to the current block being written to
//
//------------------------------------------------------------------------------
// spStorageBlock()
//
// Define our storage block object. This is used as a FP like object when a bock is
// written out interactively...

// the "OLD" blocks are from the original prototype system that needs migration
class spStorageBlockOld
{

  public:
    virtual bool writeBytes(size_t sz, char *buffer) = 0;
    virtual bool readBytes(size_t sz, char *buffer) = 0;
};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorageOld
{

  public:
    // public methods to manage a block
    virtual spStorageBlockOld *beginBlock(uint16_t idBlock, size_t sz) = 0;
    virtual spStorageBlockOld *getBlock(uint16_t idBlock) = 0;
    virtual void endBlock(spStorageBlockOld *) = 0;

    virtual void resetStorage() = 0;
};

//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library

class spStorageBlock
{

  public:
    virtual bool writeBool(const char *tag, bool data) = 0;
    virtual bool writeInt8(const char *tag, int8_t data) = 0;
    virtual bool writeInt16(const char *tag, int16_t data) = 0;    
    virtual bool writeInt32(const char *tag, int32_t data) = 0;
    virtual bool writeUInt8(const char *tag, uint8_t data) = 0;
    virtual bool writeUInt16(const char *tag, uint16_t data) = 0;    
    virtual bool writeUInt32(const char *tag, uint32_t data) = 0;
    virtual bool writeFloat(const char *tag, float data) = 0;
    virtual bool writeDouble(const char *tag, double data) = 0;
    virtual bool writeString(const char *tag, const char *data) = 0;

    // Overloaded versions
    bool write(const char *tag, bool data)
    {
        return writeBool(tag, data);
    }
    bool write(const char *tag, int8_t data)
    {
        return writeInt8(tag, data);
    }
    bool write(const char *tag, int16_t data)
    {
        return writeInt16(tag, data);
    }
    bool write(const char *tag, int32_t data)
    {
        return writeInt32(tag, data);
    }
    bool write(const char *tag, uint8_t data)
    {
        return writeUInt8(tag, data);
    }
    bool write(const char *tag, uint16_t data)
    {
        return writeUInt16(tag, data);
    }
    bool write(const char *tag, uint32_t data)
    {
        return writeUInt32(tag, data);
    }
    bool write(const char *tag, float data)
    {
        return writeFloat(tag, data);
    }
    bool write(const char *tag, double data)
    {
        return writeDouble(tag, data);
    }
    bool write(const char *tag, const char * data)
    {
        return writeString(tag, data);
    }

    virtual bool valueExists(const char *tag) = 0;
    virtual bool readBool(const char *tag,  bool &value, bool defaultValue = false) = 0;
    virtual bool readInt8(const char *tag, int8_t &value, int8_t defaultValue = 0) = 0;
    virtual bool readInt16(const char *tag, int16_t &value, int16_t defaultValue = 0) = 0;    
    virtual bool readInt32(const char *tag, int32_t &value, int32_t defaultValue = 0) = 0;
    virtual bool readUInt8(const char *tag, uint8_t &value, uint8_t defaultValue = 0) = 0;
    virtual bool readUInt16(const char *tag, uint16_t &value, uint16_t defaultValue = 0) = 0;    
    virtual bool readUInt32(const char *tag, uint32_t &value, uint32_t defaultValue = 0) = 0;
    virtual bool readFloat(const char *tag, float &value, float defaultValue = NAN) = 0;
    virtual bool readDouble(const char *tag, double &value, double defaultValue = NAN) = 0;
    virtual size_t readString(const char *tag, char *data, size_t len) = 0;

    // overload reads

    bool read(const char *tag, bool &value)
    {
        return readBool(tag, value);
    };
    bool read(const char *tag, int8_t &value)
    {
        return readInt8(tag, value);
    };
    bool read(const char *tag, int16_t &value)
    {
        return readInt16(tag, value);
    };
    bool read(const char *tag, int32_t &value)
    {
        return readInt32(tag, value);
    };
    bool read(const char *tag, uint8_t &value)
    {
        return readUInt8(tag, value);
    };
    bool read(const char *tag, uint16_t &value)
    {
        return readUInt16(tag, value);
    };
    bool read(const char *tag, uint32_t &value)
    {
        return readUInt32(tag, value);
    };
    bool read(const char *tag, float &value)
    {
        return readFloat(tag, value);
    };
    bool read(const char *tag, double &value)
    {
        return readDouble(tag, value);
    };

};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorage
{

  public:

    // Methods used to bracket the save/restore transaction
    virtual bool begin(bool readonly=false) = 0;
    virtual void end(void) = 0;
    // public methods to manage a block
    virtual spStorageBlock *beginBlock(const char *tag) = 0;

    // NOTE: TODO - for eeprom version of this, the number of bytes written
    // should be kept in the block, then when it's close, written to the block
    // header -- note, you will need to delete all existing blocks when writing
    // new ...
    virtual spStorageBlock *getBlock(const char *tag) = 0;
    virtual void endBlock(spStorageBlock *) = 0;

    virtual void resetStorage() = 0;
};
