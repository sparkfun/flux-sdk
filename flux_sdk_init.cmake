#
# Copyright (c) 2022-2024, SparkFun Electronics Inc.
#
# SPDX-License-Identifier: MIT
#
# @file: flux_sdk_init.cmake

# ##################################################################################################
# flux_sdk_version()
include(${CMAKE_CURRENT_LIST_DIR}/flux_sdk_version.cmake)

# ##################################################################################################
# flux_sdk_set_platform()
#
# Set the platform for the SDK - what hardware stack is this used on?
#
macro (flux_sdk_set_platform platform)
    set(FLUX_SDK_PLATFORM ${platform})
    message("Platform:\t${FLUX_SDK_PLATFORM}")
endmacro ()

# our default library name set(FLUX_SDK_LIBRARY_NAME SparkFun_Flux)
# ##################################################################################################
# Allow the user to set the library name
macro (flux_sdk_set_library_name library_name)
    set(FLUX_SDK_LIBRARY_NAME ${library_name})
    message("Library Name:\t${FLUX_SDK_LIBRARY_NAME}")
endmacro ()
# ##################################################################################################
# flux_sdk_set_project_directory()
#
macro (flux_sdk_set_project_directory project_directory)

    set(PROJECT_FLUX_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/${project_directory}/${FLUX_SDK_LIBRARY_NAME})

    if (NOT EXISTS ${PROJECT_FLUX_DIRECTORY})
        message(STATUS "Creating directory: ${PROJECT_FLUX_DIRECTORY}")
        file(MAKE_DIRECTORY ${PROJECT_FLUX_DIRECTORY})
    endif ()
    if (NOT EXISTS ${PROJECT_FLUX_DIRECTORY}/src)
        file(MAKE_DIRECTORY ${PROJECT_FLUX_DIRECTORY}/src)
    endif ()
    if (NOT EXISTS ${PROJECT_FLUX_DIRECTORY}/src/Flux)
        file(MAKE_DIRECTORY ${PROJECT_FLUX_DIRECTORY}/src/Flux)
    endif ()

endmacro ()
# ##################################################################################################
# flux_sdk_add_module()
#
# macro to add "modules" to our list of modules to add to the build
macro (flux_sdk_add_module)
    set(list_var "${ARGN}")
    foreach (arg IN LISTS list_var)
        list(APPEND FLUX_MODULES_TO_ADD ${arg})
    endforeach ()
endmacro ()

# ##################################################################################################
# flux_sdk_skip_module()
#
# macro to add "modules" to ignore list - helpful will setting 'add all', but wanting to skip a few
# modules.
macro (flux_sdk_skip_module)
    set(list_var "${ARGN}")
    foreach (arg IN LISTS list_var)
        list(APPEND FLUX_MODULES_TO_SKIP ${arg})
    endforeach ()
endmacro ()

function (flux_sdk_get_directory_name result_name)
    get_filename_component(THIS_MODULE ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(${result_name}
        ${THIS_MODULE}
        PARENT_SCOPE)
endfunction ()

# ##################################################################################################
# flux_sdk_add_source_files()
#
# Macro to add a file to our build system file list
macro (flux_sdk_add_source_files)
    set(list_var "${ARGN}")
    foreach (arg IN LISTS list_var)
        configure_file(${arg} ${PROJECT_FLUX_DIRECTORY}/src/Flux COPYONLY)
    endforeach ()
endmacro ()

# ##################################################################################################
# flux_sdk_is_module_enabled()
#
# Define a function that will check if a module is enabled in the list of enabled modules

function (flux_sdk_is_module_enabled module_name result)
    if (${module_name} IN_LIST FLUX_MODULES_TO_ADD)
        set(${result}
            TRUE
            PARENT_SCOPE)
    else ()
        set(${result}
            FALSE
            PARENT_SCOPE)
    endif ()
endfunction ()

# ##################################################################################################
# flux_sdk_process_subdirectories()
#
# Define a function that will cacade down subdirectories if that directory is a module desired, and
# has a CMakeLists.txt file.
#
# Note: If the <directory>_all_modules flag is set, then all submodules of that directory will be
# processed

function (flux_sdk_process_subdirectories)
    cmake_parse_arguments(ARG "DISABLE_ALL" "" "" ${ARGN})

    # Disable the use of the all flags?
    if(ARG_DISABLE_ALL)
        message("Verbose mode on")
    else()
        message("Verbose mode off")
    endif()

    # get our place in the SDK - print a value
    file(RELATIVE_PATH local_path ${FLUX_SDK_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
    message("Processing:\t\$FLUX_SDK/${local_path}")

    # Get the name of the current directory - basename. We use this to build our "all submodules"
    # flag for this directory
    get_filename_component(CurrentDir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(all_modules_flag "${CurrentDir}_all_modules")

    # is the flag set?
    flux_sdk_is_module_enabled(${all_modules_flag} process_all_submodules)

    # is everything enabled?
    flux_sdk_is_module_enabled(flux_all_modules process_all_modules)

    message("Module flag:\t${all_modules_flag} = ${process_all_submodules}")

    if(ARG_DISABLE_ALL)
        set(load_all_modules FALSE)
    elseif (process_all_submodules OR process_all_modules)
        set(load_all_modules TRUE)
    endif ()


    if ( load_all_modules)
        message("LOAD ALL MODS = TRUE")
    else()
        message("LOAD ALL MODS = FALSE")
    endif ()

    # Get all the children of this directory
    file(
        GLOB children
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/*)

    # Iterate over all the children
    foreach (child ${children})

        # Sanity check
        if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child}
            AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)

            # in the exclude list?
            if (${child} IN_LIST FLUX_MODULES_TO_SKIP)
                message(STATUS "Skipping: ${child}")
                continue ()
            endif ()
            # add this module - in list, or all devices flag set
            if (load_all_modules OR ${child} IN_LIST FLUX_MODULES_TO_ADD)
                message(STATUS "Adding: ${child}")
                add_subdirectory(${child})
                # testing options
                string(TOUPPER ${child} child_uppercase)
                set(option_name "CONFIG_${child_uppercase}")
                message(STATUS "Option name:\t${option_name}")
                option(${option_name} "Enable ${child} module" ON)
            endif ()
        endif ()
    endforeach ()
    message("")
endfunction ()

# ##################################################################################################
# flux_sdk_init()
#
# Called to start the SDK build process. This should be called after the flux_sdk import cmake file
# is loaded (of course)
macro (flux_sdk_init)
    if (NOT CMAKE_PROJECT_NAME)
        message(WARNING "flux_sdk_init() should be called after the project is created")
    endif ()

    # write some info!
    message("\n-----------------------------------------------------")
    message("SparkFun Flux SDK - Version: ${FLUX_SDK_VERSION}")
    message("-----------------------------------------------------\n")
    message("SDK Path:\t${FLUX_SDK_PATH}")
    message("Project Name:\t${CMAKE_PROJECT_NAME}")
    message("Destination:\t${PROJECT_FLUX_DIRECTORY}")
    message("")

    # is everything enabled?
    flux_sdk_is_module_enabled(flux_all_modules process_all_modules)
    message("Module flag:\tflux_all_modules = ${process_all_modules}")

    if (NOT DEFINED FLUX_SDK_PLATFORM)
        message(
            FATAL_ERROR
                "No platform specified. Set the target platform using `flux_sdk_set_platform()` in your projects CMakeLists.txt file"
        )
    endif ()
    message("Platform:\t${FLUX_SDK_PLATFORM}")

    string(TIMESTAMP COMPILE_TIME "%Y-%m-%d %H:%M:%S")
    message("Build Time:\t${COMPILE_TIME}")
    message("")

    # load the root directory of the SDK
    add_subdirectory(${FLUX_SDK_PATH} flux-sdk)

    ## Testing -- build and copy in a config file for the system.
    string(TIMESTAMP FLUX_CONFIG_BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S UTC" UTC)
    
    configure_file(${FLUX_SDK_PATH}/config/flux_config.h.in ${PROJECT_FLUX_DIRECTORY}/src/flux_config.h)

endmacro ()
