#
# Copyright (c) 2022-2024, SparkFun Electronics Inc.
#
# SPDX-License-Identifier: MIT
#
# Add the source files for this directory
flux_sdk_add_source_files(flxSettings.h flxSettings.cpp flxStorage.cpp flxStorage.h
                          flxPreferences.h)

# if the platform is not ESP32, we use our built in preference/key-value-pair functionality
if (NOT ${FLUX_SDK_PLATFORM} STREQUAL "platform_esp32")
    flux_sdk_add_source_files(
        flxKVPStore.cpp
        flxKVPStore.h
        flxKVPStoreDefs.h
        flxKVPStoreDevice.h
        flxKVPStoreEntry.cpp
        flxKVPStoreEntry.h
        flxKVPStorePage.cpp
        flxKVPStorePage.h
        flxKVPStorePrefs.h
        flxStorageKVPPref.cpp
        flxStorageKVPPref.h)

endif ()
