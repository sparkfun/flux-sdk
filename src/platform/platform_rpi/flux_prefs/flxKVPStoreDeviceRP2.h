
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
#pragma once

#include "flxKVPStoreDevice.h"
#include <cstddef>
#include <cstdint>

class flxKVPStoreDeviceRP2 : public flxKVPStoreDevice
{
  public:
    flxKVPStoreDeviceRP2();
    flxKVPStoreDeviceRP2(uint8_t *partitionStart, uint32_t segmentSize, uint32_t nSegments);
    ~flxKVPStoreDeviceRP2();

    void initialize(uint8_t *partitionStart, uint32_t segmentSize, uint32_t nSegments);
    bool write(uint32_t iPage, uint32_t address, const void *src, size_t len);

    bool read(uint32_t iPage, uint32_t address, void *dest, size_t len);

    bool erase(uint32_t iPage);

    void flush(void);
    void close(void);

    uint32_t storageSize();
    uint32_t segmentSize();

  private:
    bool setCurrentPage(uint32_t);
    void commitPage(void);
    // the pointer to the FLASH partition on the rp2*

    uint8_t *_pPartition;
    uint8_t *_pData;

    uint32_t _currentPage;
    bool _isDirty;

    uint32_t _segmentSize;
    uint32_t _nSegments;
};
