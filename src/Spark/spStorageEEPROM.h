/*
 * spStorage.h
 *
 * Class to encapsulate storage access to the devices EEProm
 */

#pragma once

#include <Arduino.h>

#include "spCoreLog.h"
#include "spStorage.h"

class spStorageEEPROM;

//------------------------------------------------------------------------------
// Storage is "block" based - data blobs are stored with headers to form a block.
//
// The blocks are connected using a linked-list pattern - influence from how TIFF
// files work. There is a header + data.
//
// Block header:

typedef struct
{
    uint16_t id;   // ID for this block
    uint16_t size; // Size in bytes of the blocks data segment
    uint16_t next; // Offset in bytes to the next block
} spBlockHeaderEEPROM;

//------------------------------------------------------------------------------
// Define our storage block object. This is used as a FP like object when a bock is
// written out interatively...

class spStorageBlockEEPROM : public spStorageBlock
{

  public:
    spStorageBlockEEPROM() : _position{0}, _locked{false}, _storage{nullptr} {};

    bool writeBytes(size_t sz, char *buffer);
    bool readBytes(size_t sz, char *buffer);

  private:
    friend spStorageEEPROM;


    void setStorage(spStorageEEPROM* theStorage)
    {
      _storage = theStorage;
    }

    spBlockHeaderEEPROM header;

    int _position; // current position in the block's data blob
    bool _locked;
    spStorageEEPROM * _storage;  ; // storage class will adjust parameters of the block
};

//------------------------------------------------------------------------------
class spStorageEEPROM : public spStorage
{

  public:
    // this is a singleton
    static spStorageEEPROM &get(void)
    {
        static spStorageEEPROM instance;
        return instance;
    }

    // public methods to manage a block
    spStorageBlockEEPROM *beginBlock(uint16_t idBlock, size_t sz);
    spStorageBlockEEPROM *getBlock(uint16_t idBlock)
    {
      return beginBlock(idBlock, 0); 
    }    

    void endBlock(spStorageBlockEEPROM *);
    void endBlock(spStorageBlock *);

    void resetStorage();

    // delete the copy and assignment constructors
    spStorageEEPROM(spStorageEEPROM const &) = delete;
    void operator=(spStorageEEPROM const &) = delete;

  private:
    friend spStorageBlockEEPROM;

    bool writeBytes(spStorageBlockEEPROM *, size_t, char *);
    bool readBytes(spStorageBlockEEPROM *, size_t, char *);

    bool validStorage(void);
    void initStorage(void);

    void write_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void write_bytes(uint16_t startPos, T &data);

    void read_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void read_bytes(uint16_t startPos, T &pBytes);

    void initialize();
    spStorageEEPROM() : _initialized{false}
    {
        initialize();
    };

    uint16_t getBlockHeader(uint16_t idTarget, size_t szBlock, spBlockHeaderEEPROM &outBlock);
    void deleteBlock(uint16_t idTarget);
    uint16_t findBlock(uint16_t idTarget, spBlockHeaderEEPROM &outBlock);

    // The block used to interface with the system
    spStorageBlockEEPROM _theBlock;

    bool _initialized;
};
