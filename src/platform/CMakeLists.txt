# Determine which port we are supporting. There can only one platform
if (NOT DEFINED FLUX_SDK_PLATFORM)
    message(
        FATAL_ERROR
            "Specify the platform to build for this project using flux_sdk_set_platform() in your projects CMakeLists.txt file"
    )
endif ()

if (${FLUX_SDK_PLATFORM} STREQUAL "platform_esp32")
    add_subdirectory(platform_esp32)
elseif (${FLUX_SDK_PLATFORM} STREQUAL "platform_rpi")
    add_subdirectory(platform_rpi)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()
