
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

#include "flxKVPStore.h"
#include "flxKVPStoreEntry.h"

flxKVPError_t flxKVPStore::checkNameSpaces(void)
{
    // Is this item in the page?
    flxKVPStoreEntry theEntry;
    uint32_t idxEntry;

    KVPNameSpaceEntry *pNSEntry;
    uint8_t iEntry;

    // Any name space entries in our pages.

    for (auto thePage : _pages)
    {
        idxEntry = 0;
        while (true)
        {
            // no entry - next page
            if (thePage->findEntry(kKVPNameSpaceEntryNS, nullptr, theEntry, idxEntry) != kKVPErrorOK)
                break;

            idxEntry++; // Will start search at next index;

            // in iEntry we have the namespace index
            theEntry.getValue(iEntry);

            // already have this NS?
            if (_nsState.test(iEntry))
                continue;

            // Okay, we have a namespace entry - add it
            pNSEntry = new KVPNameSpaceEntry;

            if (pNSEntry == nullptr)
                return kKVPErrorAlloc;

            pNSEntry->index = iEntry;
            // flxLog_I("FOUND: Adding Name Space: %s %u", theEntry.entryKey, iEntry);
            _nsState.set(iEntry);
            theEntry.getKey(pNSEntry->name, sizeof(pNSEntry->name));

            _namespaces.push_back(pNSEntry);
        }
    }
    return kKVPErrorOK;
}

flxKVPError_t flxKVPStore::getNameSpaceIndex(const char *szNS, uint8_t &outNSIndex)
{
    // Are we initalized
    if (_pages.size() == 0)
    {
        if (initialize() != kKVPErrorOK)
            return kKVPErrorConfig;
    }
    // flxLog_I("getNameSpaceIndex Name Space: %s", szNS);
    // does this name space already exist?
    auto it = std::find_if(_namespaces.begin(), _namespaces.end(), [=](KVPNameSpaceEntry *entry) -> bool {
        return strncmp(szNS, entry->name, sizeof(entry->name) - 1) == 0;
    });

    if (it != _namespaces.end())
        outNSIndex = (*it)->index;
    else
    {

        if (_pages.size() == 0 || _currPage == kNullPage)
            return kKVPErrorPageFull;

        // any room to add an namespace
        if (_nsState.all())
            return kKVPErrorOutOfRange; // close enough

        // Find an index for the namespace
        for (outNSIndex = 1; outNSIndex < _nsState.size(); outNSIndex++)
        {
            if (!_nsState.test(outNSIndex))
                break;
        }
        // flxLog_I("CREATE: Adding Name Space Index: %s %u", szNS, outNSIndex);
        flxKVPError_t retval = _pages[_currPage]->setValue(kKVPNameSpaceEntryNS, szNS, outNSIndex);

        if (retval != kKVPErrorOK)
            return retval;

        // okay - we are ready to add this to the system
        //
        KVPNameSpaceEntry *pNSEntry = new KVPNameSpaceEntry;

        if (pNSEntry == nullptr)
            return kKVPErrorAlloc;

        pNSEntry->index = outNSIndex;
        strncpy(pNSEntry->name, szNS, sizeof(pNSEntry->name));
        pNSEntry->name[sizeof(pNSEntry->name) - 1] = '\0';
        _namespaces.push_back(pNSEntry);
        _nsState.set(outNSIndex);
    }
    // flxLog_I("Name Space Index: %u", outNSIndex);
    return kKVPErrorOK;
}

//----------------------------------------------------------

flxKVPError_t flxKVPStore::initialize(void)
{
    // got storage?
    if (_storageDevice == nullptr)
        return kKVPErrorConfig;

    //
    // Determine how manage pages we need
    uint8_t nPages = _storageDevice->storageSize() / _storageDevice->segmentSize();

    if (!nPages)
        return kKVPErrorOutOfRange;

    flxKVPError_t retval;
    flxKVPStorePage *pPage;

    for (uint32_t i = 0; i < nPages; i++)
    {
        pPage = new flxKVPStorePage;
        if (pPage == nullptr)
            return kKVPErrorAlloc;

        if (!pPage->initialize(_storageDevice, i))
        {
            delete pPage;
            return kKVPErrorConfig;
        }

        retval = pPage->loadPage();
        if (retval != kKVPErrorOK)
        {
            delete pPage;
            return retval;
        }
        _pages.push_back(pPage);
    }

    // need to set the current page
    _currPage = (_pages.size() > 0 ? 0 : kNullPage);

    // Now check name spaces. First, mark off the zero space -- because
    _nsState.set(0, true);

    if (checkNameSpaces() != kKVPErrorOK)
        return kKVPErrorConfig;

    return kKVPErrorOK;
}
//----------------------------------------------------------
// Public Interface methods ...
//----------------------------------------------------------
// Get the handle for a namespace. Return 0 on error

uint8_t flxKVPStore::getNameSpace(const char *szNS)
{
    if (szNS == nullptr || strlen(szNS) < 2)
        return 0; // invalid namespace

    if (_pages.size() == 0)
    {
        if (initialize() != kKVPErrorOK)
            return 0;
    }

    uint8_t iNS;
    if (getNameSpaceIndex(szNS, iNS) != kKVPErrorOK)
        return 0;

    return iNS;
}

//----------------------------------------------------------
bool flxKVPStore::moveToFreePage(void)
{
    if (_currPage == kNullPage && _pages.size() > 0)
    {
        _currPage = 0;
        return true;
    }
    // no page to switch to?
    if (_pages.size() < 2)
        return false;

    for (size_t i = 0; i < _pages.size(); i++)
    {
        if (_pages[i]->status() == flxKVPPageStatus::kPageAvailable && (int)i != _currPage)
        {
            commit();
            _currPage = i;
            return true;
        }
    }

    return false;
}
//-----------------------------------------------------------
// setValue
flxKVPError_t flxKVPStore::setValue(uint8_t iNS, flxDataType_t dType, const char *szKey, const void *value,
                                    size_t valueSize)
{

    if (iNS < 1 || szKey == nullptr || strlen(szKey) < 2 || value == nullptr || valueSize == 0)
        return kKVPErrorBadParam;

    if (_currPage == kNullPage || _pages.size() == 0)
        return kKVPErrorPageFull;

    flxKVPError_t retval = _pages[_currPage]->setValue(iNS, dType, szKey, value, valueSize);

    // first attempt - multi tries here?
    if (retval == kKVPErrorPageFull && _pages.size() > 1)
    {
        if (moveToFreePage())
            retval = _pages[_currPage]->setValue(iNS, dType, szKey, value, valueSize);
    }

    return retval;
}

//----------------------------------------------------------
flxKVPError_t flxKVPStore::setValueString(uint8_t iNS, const char *szKey, const char *value, size_t valueSize)
{
    if (iNS < 1 || szKey == nullptr || strlen(szKey) < 2 || value == nullptr || valueSize == 0)
        return kKVPErrorBadParam;

    if (_currPage == kNullPage || _pages.size() == 0 || _pages[_currPage]->status() != flxKVPPageStatus::kPageAvailable)
        return kKVPErrorPageFull;

    // TODO: Manage page availability

    flxKVPError_t retval = _pages[_currPage]->setValueString(iNS, szKey, value, valueSize);

    // first attempt - multi tries here?
    if (retval == kKVPErrorPageFull && _pages.size() > 1)
    {
        if (moveToFreePage())
            retval = _pages[_currPage]->setValueString(iNS, szKey, value, valueSize);
    }

    return retval;
}

//----------------------------------------------------------
flxKVPError_t flxKVPStore::getValue(uint8_t iNS, flxDataType_t dType, const char *szKey, void *value, size_t valueSize)
{
    if (iNS < 1 || szKey == nullptr || strlen(szKey) < 2 || value == nullptr || valueSize == 0)
        return kKVPErrorBadParam;

    // Serial.printf("getValue (1): NS: %u, %s\n\r", iNS , szKey);
    for (auto thePage : _pages)
    {
        // Serial.printf("keyExists: %d\n\r", (int)thePage->keyExists(iNS, szKey));
        if (thePage->keyExists(iNS, szKey))
            return thePage->readValue(iNS, dType, szKey, value, valueSize);
    }
    // Serial.printf("getValue (2): %s\n\r", szKey);
    return kKVPErrorNoMatch;
}
//----------------------------------------------------------
flxKVPError_t flxKVPStore::deleteValue(uint8_t iNS, const char *szKey)
{
    if (iNS < 1 || szKey == nullptr || strlen(szKey) < 2)
        return kKVPErrorBadParam;

    for (auto thePage : _pages)
    {
        if (thePage->keyExists(iNS, szKey))
            return thePage->deleteValue(iNS, szKey);
    }

    return kKVPErrorNoMatch;
}
//----------------------------------------------------------
bool flxKVPStore::keyExists(uint8_t iNS, const char *szKey)
{
    if (iNS < 1 || szKey == nullptr || strlen(szKey) < 2)
        return false;

    for (auto thePage : _pages)
    {
        if (thePage->keyExists(iNS, szKey))
            return true;
    }
    return false;
}

//----------------------------------------------------------
void flxKVPStore::reset(void)
{
    for (auto thePage : _pages)
    {
        // Reset the page - erase it and then re-init format
        thePage->initPage(true);
    }
}