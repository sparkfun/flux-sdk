/*
 * spStorage.h
 *
 * Class to encapsulate storage access to the devices EEProm
 */

#pragma once

#include <Arduino.h>

class spStorage_;

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
} spBlockHeader;

//------------------------------------------------------------------------------
// Define our storage block object. This is used as a FP like object when a bock is
// written out interatively...

class spStorageBlock
{

  public:
    spStorageBlock() : _position(0), _locked(false){};

    bool writeBytes(size_t sz, char *buffer);
    bool readBytes(size_t sz, char *buffer);

  private:
    friend spStorage_; // storage class will adjust parameters of the block

    spBlockHeader header;

    int _position = 0; // current position in the block's data blob
    bool _locked = false;
};

//------------------------------------------------------------------------------
class spStorage_
{

  public:
    // this is a singleton
    static spStorage_ &getInstance(void)
    {
        static spStorage_ instance;
        return instance;
    }

    // public methods to manage a block
    spStorageBlock *beginBlock(uint16_t idBlock, size_t sz);
    void endBlock(spStorageBlock *);

    // delete the copy and assignment constructors
    spStorage_(spStorage_ const &) = delete;
    void operator=(spStorage_ const &) = delete;

  private:
    friend spStorageBlock;

    bool writeBytes(spStorageBlock *, size_t, char *);
    bool readBytes(spStorageBlock *, size_t, char *);

    bool validStorage(void);
    void initStorage(void);

    void write_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void write_bytes(uint16_t startPos, T &data);

    void read_bytes(uint16_t startPos, size_t sz, char *pBytes);
    template <typename T> void read_bytes(uint16_t startPos, T &pBytes);

    void initialize();
    spStorage_()
    {
        initialize();
    };

    uint16_t getBlockHeader(uint16_t idTarget, size_t szBlock, spBlockHeader &outBlock);
    void deleteBlock(uint16_t idTarget);
    uint16_t findBlock(uint16_t idTarget, spBlockHeader &outBlock);
};

// Create an accessor for the Storage class

typedef spStorage_ &spStorage;

#define spStorage() spStorage_::getInstance()
