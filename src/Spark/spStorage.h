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
