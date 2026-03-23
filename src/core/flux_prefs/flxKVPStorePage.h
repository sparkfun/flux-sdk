
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

// For a page in the storage system.

// A page is mapped to a flash IC segment

// for now, assuming the size of the segment is 4K

#pragma once

#include "flxKVPStoreDefs.h"
#include "flxKVPStoreDevice.h"
#include "flxKVPStoreEntry.h"
// page state enum

enum flxKVPPageStatus : uint32_t
{

    // Uninitialized
    kPageUninitialized = 0xFFFFFFFF,

    // Available
    kPageAvailable = 0x02,

    // Full  -- no space available

    kPageFull = 0x04,

    // page hasn't been loaded from flash
    kPageInvalid = 0
};
class flxKVPStorePage
{
  private:
    static constexpr uint32_t kNBookKeepingEntries = 2;

  public:
    static constexpr uint32_t kPageInitialized = flxKVPPageStatus::kPageAvailable;
    static constexpr uint32_t kPageFull = flxKVPPageStatus::kPageFull;

    // Page constants -- will need adjustment at some point based on deployment and environment
    //

    static constexpr uint32_t kSectorSize = 4096;

    static constexpr uint32_t kNEntriesPerPage = (kSectorSize / flxKVPStoreEntry::kEntrySize) - kNBookKeepingEntries;

    static constexpr uint32_t kNoSector = 0xFFFFFFFF;

    flxKVPStorePage();

    // called to load the page from flash
    // TBD - flash device interface needs to be provided here ..

    bool initialize(flxKVPStoreDevice *theDevice, uint32_t sectorNumber);

    flxKVPError_t loadPage(void);

    flxKVPError_t writeEntry(const flxKVPStoreEntry &theEntry);
    flxKVPError_t readEntry(uint32_t index, flxKVPStoreEntry &theEntry);

    // Version of find that enables iterating over the page - caller provides a start and end index
    flxKVPError_t findEntry(uint8_t iNS, const char *szKey, flxKVPStoreEntry &theEntry, uint32_t &entryIndex);

    // no index version
    flxKVPError_t findEntry(uint8_t iNS, const char *szKey, flxKVPStoreEntry &theEntry)
    {
        uint32_t index = 0;
        return findEntry(iNS, szKey, theEntry, index);
    };

    flxKVPError_t deleteEntry(uint32_t index);
    flxKVPError_t updateEntry(uint32_t index, const flxKVPStoreEntry &theEntry);

    //-----------------------------------------------------------------------
    // setValue
    flxKVPError_t setValue(uint8_t iNS, flxDataType_t dType, const char *szKey, const void *value, size_t valueSize);

    // Strings need their own method entries -- to catch the value type and get length correct
    flxKVPError_t setValue(uint8_t iNS, const char *szKey, const char *value)
    {
        return setValueString(iNS, szKey, value, strlen(value));
    }
    flxKVPError_t setValue(uint8_t iNS, const char *szKey, char *value)
    {
        return setValueString(iNS, szKey, (const char *)value, strlen(value));
    }
    template <typename T> flxKVPError_t setValue(uint8_t iNS, const char *szKey, T &value)
    {
        return setValue(iNS, flxGetTypeOf(value), szKey, &value, sizeof(value));
    }

    flxKVPError_t setValueString(uint8_t iNS, const char *szKey, const char *value, size_t valueSize);

    //-----------------------------------------------------------------------
    // read value methods - leverage a template to get the type of the value
    flxKVPError_t readValue(uint8_t iNS, flxDataType_t dType, const char *szKey, void *value, size_t valueSize);

    template <typename T> flxKVPError_t readValue(uint8_t iNS, const char *szKey, T &value)
    {
        return readValue(iNS, getTypeOf(value), szKey, &value, sizeof(value));
    }

    flxKVPError_t readValue(uint8_t iNS, const char *szKey, char *value, size_t len)
    {
        return readValue(iNS, flxTypeString, szKey, (void *)value, len);
    }

    flxKVPError_t deleteValue(uint8_t iNS, const char *szKey);

    bool keyExists(uint8_t iNS, const char *szKey);

    flxKVPPageStatus status()
    {
        return _pageStatus;
    }

    // Init the page - and optionally erase it
    flxKVPError_t initPage(bool bErase = false);

    void dumpPage(void);

  private:
    // Methods for our entry state table

    enum class entryStateT : uint8_t
    {
        entryEmpty = 0,
        entryError = 0x3,
        entryWritten = 0x1
    };

    entryStateT entryState(uint32_t entry)
    {

        if (entry >= kNEntriesPerPage)
            return entryStateT::entryError;

        // size_t idx = entry /16;    // 2 bits per state - 16 state / word
        // size_t shift = (entry % 16) * 2;

        return static_cast<entryStateT>((_entryState[entry / 16] >> ((entry % 16) * 2)) & 0x3);
    }

    //---------------------------------------------------------------
    entryStateT setEntryState(uint32_t entry, entryStateT eState)
    {
        if (entry >= kNEntriesPerPage)
            return entryStateT::entryError;

        size_t idx = entry / 16;
        size_t offset = (entry % 16) * 2;

        _entryState[idx] = (_entryState[idx] & ~(0x3 << offset)) | (static_cast<uint32_t>(eState) << offset);

        bool bStatus = false;
        if (_pStorage)
            bStatus = _pStorage->write(_pageSector, _pageBaseAddress + flxKVPStoreEntry::kEntrySize, _entryState,
                                       sizeof(_entryState));

        // If we are unable to write the entry state, we return an error
        return bStatus ? eState : entryStateT::entryError;
    }

    uint32_t getNextFreeEntry(uint8_t span = 1);

    //---------------------------------------------------------------
    // struct for our header -- 32 bytes in size
    /**
     * @brief Structure representing the header of a flash page.
     */
    struct flxKVPStorePageHeader
    {
        /**
         * @brief Default constructor.
         * Initializes the status to kPageInvalid and fills the fill array with 0xff.
         */
        flxKVPStorePageHeader() : status{flxKVPPageStatus::kPageInvalid}
        {
            memset(fill, 0xff, sizeof(fill) / sizeof(fill[0]));
        };

        flxKVPPageStatus status; /**< The status of the flash page. */
        uint32_t number;         /**< The number of the flash page. */
        uint8_t version;         /**< The version of the flash page. */
        uint8_t fill[19];        /**< An array used for padding. */
        uint32_t crc32;          /**< The CRC32 checksum of the flash page. */

        /**
         * @brief Calculates the CRC32 checksum of the flash page.
         * @return The calculated CRC32 checksum.
         */
        uint32_t calculateCRC32() const;
    };

    flxKVPError_t updatePageStatus(flxKVPPageStatus newStatus, bool bForce = false);

    flxKVPPageStatus _pageStatus;

    uint32_t _pageSector;
    uint32_t _pageBaseAddress;

    flxKVPStoreDevice *_pStorage;

    uint32_t _entryState[8]; // 32 bits

    uint32_t _lastEmptyEntry;
};