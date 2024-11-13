#
# Copyright (c) 2022-2024, SparkFun Electronics Inc.
#
# SPDX-License-Identifier: MIT
#
# $id$ FLUX_SDK_PATH/CMakeLists.txt
#

# we are building a "fake" Arduino library so we can compile the code using arduino-cli. This
# requires a library.properties file in the root of the project. This file i
configure_file(library.properties ${PROJECT_FLUX_DIRECTORY} COPYONLY)

# Now just cascade down to src

add_subdirectory(src)
