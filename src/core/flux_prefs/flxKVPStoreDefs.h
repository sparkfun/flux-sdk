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

/**
 * @file flxKVPStoreDefs.h
 * @brief Defs for the Key Value Pair storage .
 */

#pragma once

#include <cstddef>
#include <cstdint>

const uint8_t kKVPStoreVersion = 1;

enum flxKVPError_t : std::int8_t
{
    kKVPErrorOK = 0x00,
    kKVPErrorIO = -1,
    kKVPErrorKey = -2,
    kKVPErrorCorrupt = -3,
    kKVPErrorNamespace = -4,
    kKVPErrorPageFull = -5,
    kKVPErrorNoValue = -6,
    kKVPErrorBuffer = -7,
    kKVPErrorConfig = -8,
    kKVPErrorInvalidIndex = -9,
    kKVPErrorNoMatch = -10,
    kKVPErrorBadType = -11,
    kKVPErrorOutOfRange = -12,
    kKVPErrorAlloc = -13,
    kKVPErrorBadParam = -14,
    kKVPErrorGeneric = -99
};

// constant index for NS values

const uint8_t kKVPNameSpaceEntryNS = 0;

// Maximum length of a key name
const size_t kKVPMaxKeyNameLength = 16;