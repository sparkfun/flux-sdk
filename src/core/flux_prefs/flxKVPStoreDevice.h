
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

#include <cstddef>
#include <cstdint>

// Define our device interface
class flxKVPStoreDevice
{
  public:
    virtual ~flxKVPStoreDevice() {};

    virtual bool write(uint32_t iPage, uint32_t address, const void *src, size_t len) = 0;

    virtual bool read(uint32_t iPage, uint32_t address, void *dest, size_t len) = 0;

    virtual bool erase(uint32_t iPage) = 0;

    virtual void flush(void) = 0;
    virtual void close(void) = 0;

    virtual uint32_t storageSize() = 0;
    virtual uint32_t segmentSize() = 0;
};
