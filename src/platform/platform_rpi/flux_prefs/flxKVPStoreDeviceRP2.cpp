
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
#include "flxKVPStoreDeviceRP2.h"

#include <Arduino.h>
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <string.h>

// Memory mapped start point for the _EEPROM partition for flash
//
// General Notes
// Note on sizing of the flash partition - what is in for the rp2040 Arduino defaults
//		EEPROM = 275208576.  = 0x10FFF000
//		On the rp2040 - Flash origin in 0x10000000
//		EEPROM is at 0xFFF000 = 16773120 => 4096 * 4095
//		Note: 16MiB = 16777216 Bytes - 4096 = **1673120**
//
//		SO: The flash block for eeprom is the last 4K block in flash
//
//	also see:
//	https://petewarden.com/2024/01/16/understanding-the-raspberry-pi-picos-memory-layout/
//

const uint32_t kRP2040StorageSize = 4096;
const uint32_t kRP2040SegmentSize = 4096;
// TODO:
//    - Make this a per page thing - map 4096 to this
//    - Add cache and dirty ops
//    - Add commit method
//
const uint32_t kPartitionPageSize = kRP2040SegmentSize;

const uint32_t kPageNumberNull = 0xFFFFFFFF;

extern "C" uint8_t _EEPROM_start;

flxKVPStoreDeviceRP2::flxKVPStoreDeviceRP2() : _pData{nullptr}, _currentPage{kPageNumberNull}, _isDirty{false}
{
    _pPartition = nullptr;
    _segmentSize = 0;
    _nSegments = 0;
}
flxKVPStoreDeviceRP2::flxKVPStoreDeviceRP2(uint8_t *partitionStart, uint32_t segmentSize, uint32_t nSegments)
    : flxKVPStoreDeviceRP2()
{
    initialize(partitionStart, segmentSize, nSegments);
}

flxKVPStoreDeviceRP2::~flxKVPStoreDeviceRP2()
{
    if (!_pData)
        delete _pData;
}

void flxKVPStoreDeviceRP2::initialize(uint8_t *partitionStart, uint32_t segmentSize, uint32_t nSegments)
{
    _pPartition = partitionStart;
    _segmentSize = segmentSize;
    _nSegments = nSegments;
}
bool flxKVPStoreDeviceRP2::setCurrentPage(uint32_t newPage)
{
    if (newPage == _currentPage)
        return true;

    // flush out current
    commitPage();

    if (_pData == nullptr)
    {
        _pData = new uint8_t[kPartitionPageSize];

        if (_pData == nullptr)
            return false;
    }

    // copy the current contents of flash for this page, into our r/w ram buffer
    memcpy(_pData, _pPartition + (newPage * kPartitionPageSize), kPartitionPageSize);

    _currentPage = newPage;

    return true;
}

void flxKVPStoreDeviceRP2::commitPage(void)
{

    if (!_isDirty || _currentPage == kPageNumberNull || !_pData)
        return;

    // This is from the rp2040 Arduino core - probably done better
    // TODO - Move this to use the flash api in the pico sdk

    // uint32_t intr_stash = save_and_disable_interrupts();
    noInterrupts(); // from the Arduino core impl
    // rp2040.idleOtherCore(); // 10/2/2024 -- KDB - this hangs the system...

    flash_range_erase((intptr_t)(_pPartition + (_currentPage * kPartitionPageSize)) - (intptr_t)XIP_BASE,
                      kPartitionPageSize);
    flash_range_program((intptr_t)(_pPartition + (_currentPage * kPartitionPageSize)) - (intptr_t)XIP_BASE, _pData,
                        kPartitionPageSize);
    // rp2040.resumeOtherCore();
    interrupts();

    // restore_interrupts(intr_stash);
    _isDirty = false;
}

bool flxKVPStoreDeviceRP2::erase(uint32_t iPage)
{
    if (!setCurrentPage(iPage))
        return false;

    memset(_pData, 0xFF, kPartitionPageSize);
    _isDirty = true;

    return true;
}

bool flxKVPStoreDeviceRP2::write(uint32_t iPage, uint32_t address, const void *src, size_t len)
{
    if (!src || len == 0 || address + len >= _segmentSize)
        return false;

    if (!setCurrentPage(iPage))
        return false;

    memcpy(_pData + address, src, len);
    _isDirty = true;

    return true;
}

bool flxKVPStoreDeviceRP2::read(uint32_t iPage, uint32_t address, void *dest, size_t len)
{
    if (!dest || len == 0 || address + len >= _segmentSize)
        return false;

    if (!setCurrentPage(iPage))
        return false;
    memcpy(dest, _pData + address, len);

    return true;
}

void flxKVPStoreDeviceRP2::flush(void)
{
    commitPage();
}

void flxKVPStoreDeviceRP2::close(void)
{
    commitPage();

    if (_pData)
    {
        delete[] _pData;
        _pData = nullptr;
    }
    _currentPage = kPageNumberNull;
}

uint32_t flxKVPStoreDeviceRP2::storageSize(void)
{
    return kRP2040StorageSize;
}

uint32_t flxKVPStoreDeviceRP2::segmentSize(void)
{
    return kRP2040SegmentSize;
}