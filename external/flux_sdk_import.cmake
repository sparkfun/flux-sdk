#
# Copyright (c) 2022-2024, SparkFun Electronics Inc.
#
# SPDX-License-Identifier: MIT
#
# ##################################################################################################
# include/import file for the flux-sdk cmake system
# ##################################################################################################

# we are just using this as a manifest system, so doing the best to bypass the compiler detection
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Where is the flux-sdk directory? This should be in some "include file from the sdk"

if (NOT FLUX_SDK_PATH)
    if (DEFINED ENV{FLUX_SDK_PATH})
        set(FLUX_SDK_PATH $ENV{FLUX_SDK_PATH})
    elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../flux-sdk)
        set(FLUX_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../flux-sdk)
    endif ()
endif ()

# crawl the flux-sdk directory and add all the subdirectories
include(${FLUX_SDK_PATH}/flux_sdk_init.cmake)
