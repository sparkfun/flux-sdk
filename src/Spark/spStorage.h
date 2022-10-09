/*
 * spStorage.h
 *
 * Define our interfaces for state saving/storage
 */

#pragma once

#include <stddef.h>
#include <cstdint>

// Storage involves two interfaces
//
//  - spStorage       - The main interface into the storage system being used. 
//  - spStorageBlock  - Interface to the current block being written to
//
//------------------------------------------------------------------------------
// spStorageBlock()
//
// Define our storage block object. This is used as a FP like object when a bock is
// written out interatively...

class spStorageBlock
{

  public:
    virtual bool writeBytes(size_t sz, char *buffer) = 0;
    virtual bool readBytes(size_t sz, char *buffer)  = 0;
};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorage
{

  public:
    // public methods to manage a block
    virtual spStorageBlock *beginBlock(uint16_t idBlock, size_t sz) = 0;
    virtual spStorageBlock *getBlock(uint16_t idBlock) = 0;    
    virtual void endBlock(spStorageBlock *) = 0;

    virtual void resetStorage() = 0;
};

//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library

class spStorageBlock2
{

  public:

    virtual bool writeBool(const char *tag, bool data) = 0;
    virtual bool writeInt8(const char *tag, int8_t data) = 0;
    virtual bool writeInt32(const char *tag, int32_t data) = 0;        
    virtual bool writeUInt8(const char *tag, uint8_t data) = 0;        
    virtual bool writeUInt32(const char *tag, uint32_t data) = 0;        
    virtual bool writeFloat(const char *tag, float data) = 0;        
    virtual bool writeDouble(const char *tag, double data) = 0;        
    virtual bool writeString(const char *tag, char * data) = 0;                            

    virtual bool readBool(const char *tag, bool defaultValue ) = 0;
    virtual int8_t readInt8(const char *tag, int8_t defaultValue ) = 0;
    virtual int32_t readInt32(const char *tag, int32_t defaultValue ) = 0;        
    virtual uint8_t readUInt8(const char *tag, uint8_t defaultValue ) = 0;        
    virtual uint32_t readUInt32(const char *tag, uint32_t defaultValue ) = 0;        
    virtual float readFloat(const char *tag, float defaultValue ) = 0;        
    virtual double readDouble(const char *tag, double defaultValue ) = 0;        
    virtual size_t readString(const char *tag, char * data, size_t len) = 0; 

};

//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

class spStorage2
{

  public:
    // public methods to manage a block
    virtual spStorageBlock2 *beginBlock(const char *tag) = 0;

    // NOTE: TODO - for eeprom version of this, the number of bytes written
    // should be kept in the block, then when it's close, written to the block
    // header -- note, you will need to delete all existing blocks when writing 
    // new ... 
    virtual spStorageBlock2 *getBlock(const char *tag) = 0;    
    virtual void endBlock(spStorageBlock2 *) = 0;

    virtual void resetStorage() = 0;
};
