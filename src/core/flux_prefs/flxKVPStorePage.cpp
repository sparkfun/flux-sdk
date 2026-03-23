
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
#include "flxKVPStorePage.h"
#include "flxCoreLog.h"
#include "flxKVPStore.h"
#include "flxKVPStoreDefs.h"
#include "flxUtils.h"

//
uint32_t flxKVPStorePage::flxKVPStorePageHeader::calculateCRC32() const
{
    return flx_utils::calc_crc32(0xffffffff,
                                 reinterpret_cast<const uint8_t *>(this) + offsetof(flxKVPStorePageHeader, number),
                                 offsetof(flxKVPStorePageHeader, crc32) - offsetof(flxKVPStorePageHeader, number));
}

//---------------------------------------------
//
//
//
//**

flxKVPStorePage::flxKVPStorePage()
    : _pageStatus{flxKVPPageStatus::kPageInvalid}, _pageSector{kNoSector}, _pageBaseAddress{0}, _pStorage{nullptr},
      _entryState{0}, _lastEmptyEntry{0}
{
}

/**
 * Initializes the flxFPSPage object.
 *
 * @param theDevice The flxFPSDevice object to associate with this page.
 * @param sectorNumber The sector number of the page.
 * @return True if initialization is successful, false otherwise.
 */
bool flxKVPStorePage::initialize(flxKVPStoreDevice *theDevice, uint32_t sectorNumber)
{

    if (!theDevice)
        return false;

    _pStorage = theDevice;

    _pageSector = sectorNumber;

    _pageBaseAddress = kSectorSize * sectorNumber;

    _lastEmptyEntry = 0;

    return true;
}

/** @brief Updates the status of the page.
 *
 * @param newStatus The new status to set for the page.
 * @return kKVPErrorOK if the status was successfully updated, kKVPErrorConfig if the storage is not configured, or
 * kKVPErrorIO if there was an I/O error.
 */
flxKVPError_t flxKVPStorePage::updatePageStatus(flxKVPPageStatus newStatus, bool bForce /*=false*/)
{
    // Serial.printf("updatePageStatus() - newStatus: %d, sector %u, pageStatus: 0x%X\n\r", newStatus, _pageSector,
    //   _pageStatus);
    if (!bForce && newStatus == _pageStatus)
        return flxKVPError_t::kKVPErrorOK;

    _pageStatus = newStatus;

    if (!_pStorage)
        return flxKVPError_t::kKVPErrorConfig;

    flxKVPStorePageHeader theHeader;
    theHeader.status = _pageStatus;
    theHeader.number = _pageSector;
    theHeader.version = kKVPStoreVersion;
    theHeader.crc32 = theHeader.calculateCRC32();

    if (!_pStorage->write(_pageSector, _pageBaseAddress, &theHeader, sizeof(flxKVPStorePageHeader)))
    {
        // Serial.println("Error - updatePageStatus - write -  unable to update page status\n\r");
        _pageStatus = flxKVPPageStatus::kPageInvalid;
        return flxKVPError_t::kKVPErrorIO;
    }

    return flxKVPError_t::kKVPErrorOK;
}

//---------------------------------------------
/**
 * @brief Initialize the page AND write init values to flash
 *
 * @param bErase if true, erase the page - defaults to false
 * @return flxKVPError_t kKVPErrorOK if success
 */
flxKVPError_t flxKVPStorePage::initPage(bool bErase /*=false*/)
{
    // no storage device, we haven't been initialized
    if (!_pStorage || _pageSector == kNoSector)
        return kKVPErrorConfig;

    // if we are erasing, we need to do that first
    if (bErase)
    {
        if (!_pStorage->erase(_pageSector))
            return kKVPErrorIO;
    }

    // force the page to be updated in the header/storage when update is called
    flxKVPError_t retval = updatePageStatus(flxKVPPageStatus::kPageAvailable, true);

    if (retval != kKVPErrorOK)
        return retval;

    // Write out the entry state table.
    memset(_entryState, static_cast<uint8_t>(entryStateT::entryEmpty), sizeof(_entryState));

    bool bStatus = _pStorage->write(_pageSector, _pageBaseAddress + flxKVPStoreEntry::kEntrySize, _entryState,
                                    sizeof(_entryState));

    if (!bStatus)
    {
        _pageStatus = flxKVPPageStatus::kPageInvalid;
        return kKVPErrorIO;
    }

    // commit our header changes
    _pStorage->flush();

    return kKVPErrorOK;
}
//---------------------------------------------
/**
 * @brief Loads the page from the storage device.
 *
 * @return kKVPErrorOK if successful. KVPErrorConfig if the page has not been initialized. @return KVPErrorIO if
 * there was an I/O error.
 */
flxKVPError_t flxKVPStorePage::loadPage(void)
{
    // no storage device, we haven't been initialized
    if (!_pStorage)
    {
        flxLog_E(F("KVP Storage - loadPage() - no storage device"));
        return kKVPErrorConfig;
    }

    // Lets read in the header

    flxKVPStorePageHeader theHeader;
    bool bStatus = _pStorage->read(_pageSector, _pageBaseAddress, &theHeader, sizeof(flxKVPStorePageHeader));

    if (!bStatus)
    {
        flxLog_E(F("KVP Storage - unable to load page"));
        // read failed, not good
        updatePageStatus(flxKVPPageStatus::kPageInvalid);

        return kKVPErrorIO;
    }

    bool bNeedsInit = false;
    // do we need to init the page?
    if (theHeader.status != flxKVPPageStatus::kPageFull && theHeader.status != flxKVPPageStatus::kPageAvailable)
        bNeedsInit = true;
    else if (theHeader.crc32 != theHeader.calculateCRC32())
    {
        bNeedsInit = true;
    }
    // Serial.printf("loadPage() - page status: 0x%X, needs init: %d\n\r", theHeader.status, bNeedsInit);

    if (bNeedsInit)
    {
        // if we are here, we need to init the page
        if (initPage() != kKVPErrorOK)
            return kKVPErrorIO;
    }
    else
        _pageStatus = theHeader.status;

    // need to load in the entry status table.
    bStatus =
        _pStorage->read(_pageSector, _pageBaseAddress + flxKVPStoreEntry::kEntrySize, _entryState, sizeof(_entryState));
    if (!bStatus)
    {
        initPage();

        // TODO: -- Should we erase the page at this point - OR rebuild the entry table -- don't like rebuild
    }

    return kKVPErrorOK;
}

//---------------------------------------------
/**
 * @brief Get the index of the next free entry in the flash page.
 *
 * This function searches for the next free entry in the flash page's index table.
 * It starts searching from the last empty entry and returns the index of the first
 * free entry it finds. If no free entry is found, it marks the page as full and
 * returns the value flxFPSEntry::kEntryInvalid.
 *
 * @return The index of the next free entry, or flxFPSEntry::kEntryInvalid if the page is full.
 */
uint32_t flxKVPStorePage::getNextFreeEntry(uint8_t span)
{
    // blast through our index table and find a free entry that can support the span
    uint32_t idx = flxKVPStoreEntry::kEntryInvalid;

    for (uint32_t nEmpty = 0, i = 0; i < kNEntriesPerPage; i++)
    {
        if (entryState(i) == entryStateT::entryEmpty)
        {
            // First empty - set index to curent
            if (nEmpty == 0)
                idx = i;

            //  Increment the empty count. If we have enough empty entries, return the index
            if (++nEmpty == span)
                return idx;
        }
        else
            nEmpty = 0;
    }

    // if we are here, we are full
    updatePageStatus(flxKVPPageStatus::kPageFull);

    return flxKVPStoreEntry::kEntryInvalid;
}

//-------------------------------------------------------
// writeEntry()
//
/**
 * @brief Writes a flxFPSEntry to the storage device.
 *
 * @param[in] theEntry The flxFPSEntry to be written.
 * @return flxKVPError_t The error code indicating the result of the write operation. Returns kKVPErrorConfig if the
 * page has not been initialized, kKVPErrorPageFull if the page is full, or kKVPErrorIO if there was an I/O error.
 */
flxKVPError_t flxKVPStorePage::writeEntry(const flxKVPStoreEntry &theEntry)
{
    if (!_pStorage || _pageStatus == flxKVPPageStatus::kPageFull)
        return kKVPErrorConfig;

    uint32_t index = getNextFreeEntry();

    if (index == flxKVPStoreEntry::kEntryInvalid)
        return kKVPErrorPageFull;

    return updateEntry(index, theEntry);
}

//--------------------------------------------------------
// readEntry()
/**
 * @brief Reads an entry from the flash storage page.
 *
 * This function reads the entry at the specified index from the flash storage page.
 *
 * @param index The index of the entry to read.
 * @param theEntry Reference to the flxFPSEntry object to store the read entry.
 * @return The error code indicating the status of the read operation.
 *         - kKVPErrorConfig: If the storage is not initialized or the entry state is not entryWritten.
 *         - kKVPErrorIO: If there was an I/O error during the read operation.
 *         - kKVPErrorOK: If the read operation was successful.
 */
flxKVPError_t flxKVPStorePage::readEntry(uint32_t index, flxKVPStoreEntry &theEntry)
{
    if (!_pStorage || entryState(index) != entryStateT::entryWritten)
        return kKVPErrorConfig;

    uint32_t address = _pageBaseAddress + (index + kNBookKeepingEntries) * flxKVPStoreEntry::kEntrySize;

    // return status code based on bool from read.
    return _pStorage->read(_pageSector, address, &theEntry, sizeof(theEntry)) ? kKVPErrorOK : kKVPErrorIO;
}
//--------------------------------------------------------
// updateEntry()
/**
 * @brief Updates an entry in the flash page.
 *
 * This function updates the entry at the specified index with the provided entry data.
 *
 * @param index The index of the entry to be updated.
 * @param theEntry The entry data to be written.
 * @return The error code indicating the status of the update operation.
 *         - kKVPErrorConfig: If there is no storage device available.
 *         - kKVPErrorInvalidIndex: If the provided index is invalid.
 *         - kKVPErrorIO: If there was an error during the write operation.
 *         - kKVPErrorOK: If the update operation was successful.
 */
flxKVPError_t flxKVPStorePage::updateEntry(uint32_t index, const flxKVPStoreEntry &theEntry)
{
    // no storage device, we haven't been initialized
    if (!_pStorage)
        return kKVPErrorConfig;

    if (index == flxKVPStoreEntry::kEntryInvalid || index >= flxKVPStorePage::kNEntriesPerPage)
        return kKVPErrorInvalidIndex;

    uint32_t address = _pageBaseAddress + (index + kNBookKeepingEntries) * flxKVPStoreEntry::kEntrySize;

    bool bStatus = _pStorage->write(_pageSector, address, &theEntry, sizeof(theEntry));

    // if success, update the status in the entry table
    if (bStatus)
    {
        for (uint8_t i = 0; i < theEntry.span; i++)
            setEntryState(index + i, entryStateT::entryWritten);
        _lastEmptyEntry = index + theEntry.span;
    }
    // return status code based on bool from write.
    return bStatus ? kKVPErrorOK : kKVPErrorIO;
}
//--------------------------------------------------------
// findEntry()
/**
 * Finds an entry in the flxFPSPage object.
 *
 * @param iNS The namespace of the entry.
 * @param szKey The key of the entry.
 * @param theEntry The output parameter to store the found entry.
 * @param[in,out] entryIndex Input: the index of the entry to start the search from, returns the index of the found
 * entry.
 * @return The error code indicating the result of the operation. kKVPErrorConfig if the storage is not initialized,
 * kKVPErrorKey if the key is invalid, kKVPErrorInvalidIndex if the provided index is invalid, kKVPErrorIO if there
 * was an I/O error, or kKVPErrorNoMatch if no matching entry was found.
 */
flxKVPError_t flxKVPStorePage::findEntry(uint8_t iNS, const char *szKey, flxKVPStoreEntry &theEntry,
                                         uint32_t &entryIndex)
{
    // Serial.printf("findEntry() - pStorage: %p, pageSector: %u , page status 0x%X \n\r", _pStorage, _pageSector,
    //   _pageStatus);
    // no storage device, we haven't been initialized; The page is okay
    if (!_pStorage || _pageStatus == flxKVPPageStatus::kPageUninitialized ||
        _pageStatus == flxKVPPageStatus::kPageInvalid)
        return kKVPErrorConfig;

    if (szKey == nullptr && iNS != kKVPNameSpaceEntryNS)
        return kKVPErrorKey;

    // entryIndex is provided -- helps do multiple iterations of a search
    if (entryIndex >= kNEntriesPerPage)
        return kKVPErrorInvalidIndex;

    // TODO: Optimize the start point for this search

    uint32_t inc = 0;

    // Serial.printf("Starting search at index: %u - key: %s\n\r", iNS, szKey);

    for (uint32_t i = entryIndex; i < flxKVPStorePage::kNEntriesPerPage; i += inc)
    {
        inc = 1;

        // is this entry written/valid
        if (entryState(i) != entryStateT::entryWritten)
            continue;

        // read the entry
        if (readEntry(i, theEntry) != kKVPErrorOK)
            return kKVPErrorIO;

        // Is this type a string - if so, we need to skip the span of the string
        // note - handle if span is set to 0
        inc = theEntry.span == 0 ? 1 : theEntry.span;

        // data still look okay?
        if (theEntry.crc32 != theEntry.calculateCRC32())
        {
            // Delete this entry - it's corrupt
            deleteEntry(i);
            continue;
        }

        if (iNS != theEntry.iNameSpace)
            continue;

        // at this point the namespace matches. If no key - match, or  if key and key matches, we match
        if ((iNS == kKVPNameSpaceEntryNS && szKey == nullptr) ||
            strncmp(szKey, theEntry.entryKey, flxKVPStoreEntry::kMaxKeyLength) == 0)
        {
            entryIndex = i;
            return kKVPErrorOK;
        }
    }

    // if we are here - no match
    return kKVPErrorNoMatch;
}

//--------------------------------------------------------------
// Methods to set/get values
//--------------------------------------------------------------

/**
 * @brief Deletes an entry at the specified index.
 *
 * @param index The index of the entry to delete.
 * @return flxKVPError_t Returns kKVPErrorInvalidIndex if the index is invalid, kKVPErrorIO if there was an I/O
 * error, or kKVPErrorOK if the entry was successfully deleted.
 */
flxKVPError_t flxKVPStorePage::deleteEntry(uint32_t index)
{
    // is this entry written/valid?
    if (entryState(index) != entryStateT::entryWritten)
        return kKVPErrorInvalidIndex;

    // basically mark the entries for this item as free in the index
    flxKVPStoreEntry theEntry;
    if (readEntry(index, theEntry) != kKVPErrorOK)
        return kKVPErrorIO;

    // deal with the span of the entry
    uint32_t nErase = theEntry.span;

    for (uint32_t i = 0; i < nErase; i++)
        (void)setEntryState(index + i, entryStateT::entryEmpty);

    if (index < _lastEmptyEntry)
        _lastEmptyEntry = index;

    return kKVPErrorOK;
}
//--------------------------------------------------------------
//
// setString - which can span multiple records
//

/**
 * @brief Sets the value of a key-value pair in the flxFPSPage.
 *
 * @param iNS The namespace of the key-value pair.
 * @param szKey The key of the key-value pair.
 * @param value The value to be set.
 * @param valueSize The size of the value.
 * @return flxKVPError_t Returns kKVPErrorConfig if the storage is not initialized, kKVPErrorPageFull if the page is
 full, kKVPErrorIO if there was an I/O error and kKVPErrorOK if the operation was successful.

 */
flxKVPError_t flxKVPStorePage::setValueString(uint8_t iNS, const char *szKey, const char *value, size_t valueSize)
{
    if (!_pStorage)
        return kKVPErrorConfig;

    // Is this item in the page?
    flxKVPStoreEntry theEntry;
    uint32_t idxEntry = 0;

    flxKVPError_t itExists = findEntry(iNS, szKey, theEntry, idxEntry);

    // how many entries will this puppy require
    size_t totalSize = (valueSize + flxKVPStoreEntry::kEntrySize - 1) & ~(flxKVPStoreEntry::kEntrySize - 1);
    size_t nEntries = (totalSize / flxKVPStoreEntry::kEntrySize) + 1;
    totalSize = totalSize + flxKVPStoreEntry::kEntrySize;

    size_t newSpan = (totalSize + flxKVPStoreEntry::kEntrySize - 1) / flxKVPStoreEntry::kEntrySize;

    // room on this page for the string?
    if (itExists == kKVPErrorOK)
    {
        // no changes in type/value?
        if (theEntry.dataLength.dataSize == valueSize)
        {
            // If the value is the same, return kKVPErrorOK
            uint32_t address = _pageBaseAddress + (idxEntry + kNBookKeepingEntries + 1) * flxKVPStoreEntry::kEntrySize;
            if (memcmp(value, (char *)address, valueSize) == 0)
                return kKVPErrorOK;
        }

        // if the current span length being used is less than needed, delete the entry. A new one is needed.
        if (theEntry.span < newSpan)
        {
            // We can't reuse and need to expand - delete and retreat
            deleteEntry(idxEntry);
            itExists = kKVPErrorGeneric; // force a new entry later in method
        }
        else if (theEntry.span > newSpan)
        {
            // We can use the current space, but don't requires the same number of
            // entries/records. Mark the unused entries as empty
            for (uint32_t i = newSpan; i < theEntry.span; i++)
                setEntryState(idxEntry + i, entryStateT::entryEmpty);

            theEntry.span = newSpan;
        }
    }

    // do we need find a spot for this entry
    if (itExists != kKVPErrorOK)
    {
        // if we are here, we need to find space for the string
        idxEntry = getNextFreeEntry(newSpan);
        if (idxEntry == flxKVPStoreEntry::kEntryInvalid)
            return kKVPErrorPageFull;

        theEntry = flxKVPStoreEntry(iNS, flxTypeString, newSpan, szKey);
    }

    // Setup the Entry
    const uint8_t *uiValue = reinterpret_cast<const uint8_t *>(value);

    theEntry.dataLength.dataCRC32 = flxKVPStoreEntry::calculateCRC32(uiValue, valueSize);
    theEntry.dataLength.dataSize = valueSize;
    theEntry.dataLength.reserved = 0xFFFF;

    theEntry.crc32 = theEntry.calculateCRC32();
    if (updateEntry(idxEntry, theEntry) != kKVPErrorOK)
        return kKVPErrorIO;

    // now write out the string data
    uint32_t address = _pageBaseAddress + (idxEntry + kNBookKeepingEntries + 1) * flxKVPStoreEntry::kEntrySize;
    return _pStorage->write(_pageSector, address, uiValue, valueSize) ? kKVPErrorOK : kKVPErrorIO;
}

//--------------------------------------------------------------
// Set a typed value
//

/**
 * @brief Sets the value of a key-value pair in the flash page.
 *
 * @param iNS The namespace index.
 * @param dType The data type of the value.
 * @param szKey The key.
 * @param value A pointer to the value.
 * @param valueSize The size of the value.
 * @return The error code indicating the success or failure of the operation. kKVPErrorOK if successful
 */
flxKVPError_t flxKVPStorePage::setValue(uint8_t iNS, flxDataType_t dType, const char *szKey, const void *value,
                                        size_t valueSize)
{
    // Strings are different
    if (dType == flxTypeString)
        return setValueString(iNS, szKey, (const char *)value, strlen((const char *)value));

    // Is this item in the page?
    flxKVPStoreEntry theEntry;
    uint32_t idxEntry = 0;

    flxKVPError_t itExists = findEntry(iNS, szKey, theEntry, idxEntry);
    if (itExists == kKVPErrorOK)
    {
        // no changes in type/value?
        if (dType == theEntry.dataType && memcmp(value, theEntry.data, valueSize) == 0)
            return kKVPErrorOK;
    }
    else
        theEntry = flxKVPStoreEntry(iNS, dType, 1, szKey);

    // setup the entry
    memcpy(theEntry.data, value, valueSize);
    theEntry.crc32 = theEntry.calculateCRC32();

    return itExists == kKVPErrorOK ? updateEntry(idxEntry, theEntry) : writeEntry(theEntry);
}

//--------------------------------------------------------------
// Methods to set/get values
//
// Set a typed value
//

/**
 * @brief Reads the value associated with the specified key from the flash storage page.
 *
 * @param iNS The namespace of the key-value pair.
 * @param dType The data type of the value.
 * @param szKey The key to search for.
 * @param value Pointer to the buffer where the value will be stored.
 * @param valueSize The size of the buffer.
 * @return flxKVPError_t The error code indicating the result of the operation. Returns kKVPErrorNoMatch if no
 matching value found, kKVPErrorBadType if the data type does not match, kKVPErrorBuffer if the buffer is too small,
 kKVPErrorIO if there was an I/O error, or kKVPErrorCorrupt if the data is corrupt. , or kKVPErrorOK if the
 operation was successful.

 */
flxKVPError_t flxKVPStorePage::readValue(uint8_t iNS, flxDataType_t dType, const char *szKey, void *value,
                                         size_t valueSize)
{
    // Is this item in the page?
    flxKVPStoreEntry theEntry;
    uint32_t idxEntry = 0;

    if (findEntry(iNS, szKey, theEntry, idxEntry) != kKVPErrorOK)
        return kKVPErrorNoMatch;

    if (dType != theEntry.dataType)
        return kKVPErrorBadType;

    if (dType == flxTypeString)
    {
        //// Serial.printf("Sizes: %d, %d \n\r", valueSize, theEntry.dataLength.dataSize);
        if (valueSize < (size_t)theEntry.dataLength.dataSize + 1)
            return kKVPErrorBuffer;

        uint8_t *uiData = reinterpret_cast<uint8_t *>(value);

        // get the string
        uint32_t address = _pageBaseAddress + (idxEntry + kNBookKeepingEntries + 1) * flxKVPStoreEntry::kEntrySize;
        bool status = _pStorage->read(_pageSector, address, uiData, theEntry.dataLength.dataSize);

        if (!status)
            return kKVPErrorIO;

        uiData[theEntry.dataLength.dataSize] = '\0';

        // check our crc
        if (flxKVPStoreEntry::calculateCRC32(uiData, theEntry.dataLength.dataSize) != theEntry.dataLength.dataCRC32)
            return kKVPErrorCorrupt;
    }
    else
        memcpy(value, theEntry.data, valueSize);

    return kKVPErrorOK;
}
/**
 * @brief Delete a key-value pair from the flash storage page.
 *
 * This function deletes a key-value pair from the flash storage page based on the provided namespace and key.
 *
 * @param iNS The namespace of the key-value pair.
 * @param szKey The key of the key-value pair.
 * @return The result of the delete operation.
 *         - kKVPErrorOK: The key-value pair was successfully deleted.
 *         - kKVPErrorNoMatch: The key-value pair was not found in the page.
 */
flxKVPError_t flxKVPStorePage::deleteValue(uint8_t iNS, const char *szKey)
{
    // Is this item in the page?
    flxKVPStoreEntry theEntry;
    uint32_t idxEntry = 0;

    if (findEntry(iNS, szKey, theEntry, idxEntry) != kKVPErrorOK)
        return kKVPErrorNoMatch;

    return deleteEntry(idxEntry);
}

/**
 * Checks if a key exists in the flash page.
 *
 * @param iNS The namespace of the key.
 * @param szKey The key to check.
 * @return True if the key exists, false otherwise.
 */
bool flxKVPStorePage::keyExists(uint8_t iNS, const char *szKey)
{
    // Is this item in the page?
    flxKVPStoreEntry theEntry;

    // Serial.printf("Checking for key: %s\n\r", szKey);
    return findEntry(iNS, szKey, theEntry) == kKVPErrorOK;
}
//---------------------------------------------
/**
 * @brief Dumps the contents of the page to the Serial monitor.
 *
 * This method prints the page number, status, base address, last empty entry,
 * and the state of each entry in the page to the Serial monitor.
 */
void flxKVPStorePage::dumpPage(void)
{
    // TODO: Move the output to the framework methods when this is integrated into the framework

    flxLog_I(F("Page: %lu"), _pageSector);
    flxLog_I(F("Status: %lu"), _pageBaseAddress);
    flxLog_I(F("Last Empty Entry: %lu"), _lastEmptyEntry);

    for (uint32_t i = 0; i < kNEntriesPerPage; i++)
    {
        flxLog_I(F("Entry %lu: 0x%X"), i, (uint8_t)entryState(i));
    }
}