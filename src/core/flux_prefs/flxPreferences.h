/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// This defines a common preference class, using the platform specific prefs class based
// on the current platform.
//
// The choice is between the built in ESP32 prefs class or our own KVP system.
//
// This file defines a class based on the platform.

#ifdef ESP32
#include "flxStorageESP32Pref.h"
#else
#include "flxStorageKVPPref.h"
#endif

// define our class based on the platform

#ifdef ESP32
class flxPreferences : public flxStorageESP32Pref
{
};
#else
class flxPreferences : public flxStorageKVPPref
{
};
#endif
