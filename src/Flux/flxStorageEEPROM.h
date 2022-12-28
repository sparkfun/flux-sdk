/*
 * flxStorage.h
 *
 * Class to encapsulate storage access to the devices EEProm
 */

#pragma once

#include <Arduino.h>

#include "flxCoreLog.h"
#include "flxStorage.h"

class flxStorageEEPROM;

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

class flxStorageBlockEEPROM : public flxStorageBlockOld
{

  public:
    flxStorageBlockEEPROM() : _position{0}, _locked{false}, _storage{nullptr} {};

    bool writeBytes(size_t sz, char *buffer);
    bool readBytes(size_t sz, char *buffer);

  private:
    friend flxStorageEEPROM;


    void setStorage(flxStorageEEPROM* theStorage)
    {
      _storage = theStorage;
    }

    spBlockHeaderEEPROM header;

    int _position; // current position in the block's data blob
    bool _locked;
    flxStorageEEPROM * _storage;  ; // storage class will adjust parameters of the block
};

//------------------------------------------------------------------------------
class flxStorageEEPROM : public flxStorageOld
{

  public:
    // this is a singleton
    static flxStorageEEPROM &get(void)
    {
        static flxStorageEEPROM instance;
        return instance;
    }

    // public methods to manage a block
    flxStorageBlockEEPROM *beginBlock(uint16_t idBlock, size_t sz);
    flxStorageBlockEEPROM *getBlock(uint16_t idBlock)
    {
      return beginBlock(idBlock, 0); 
    }    

    void endBlock(flxStorageBlockEEPROM *);
    void endBlock(flxStorageBlockOld *);

    void resetStorage();

    // delete the copy and assignment constructors
    flxStorageEEPROM(flxStorageEEPROM const &) = delete;
    void operator=(flxStorageEEPROM const &) = delete;

  private:
    friend flxStorageBlockEEPROM;

    bool writeBytes(flxStorageBlockEEPROM *, size_t, char *);
    bool readBytes(flxStorageBlockEEPROM *, size_t, char *);

    bool validStorage(void);
    void initStorage(void);

    void write_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void write_bytes(uint16_t startPos, T &data);

    void read_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void read_bytes(uint16_t startPos, T &pBytes);

    void initialize();
    flxStorageEEPROM() : _initialized{false}
    {
        initialize();
    };

    uint16_t getBlockHeader(uint16_t idTarget, size_t szBlock, spBlockHeaderEEPROM &outBlock);
    void deleteBlock(uint16_t idTarget);
    uint16_t findBlock(uint16_t idTarget, spBlockHeaderEEPROM &outBlock);

    // The block used to interface with the system
    flxStorageBlockEEPROM _theBlock;

    bool _initialized;
};
