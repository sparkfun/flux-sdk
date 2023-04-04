/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 

// Action to mange the device/system Firmware - perform a factory reset, apply firmware updates

#pragma once

#include "flxCore.h"
#include "flxFlux.h"
#include "flxSettingsSerial.h"
#include "flxFS.h"

class flxSysFirmware : public flxActionType<flxSysFirmware>
{

private:

    void doFactoryReset(void);
    bool getFirmwareFilename(void);

    //------------------------------------------------------------------------------
    void factory_reset(const bool &doReset)
    {
        if (doReset)
            doFactoryReset();
        else
            flxLog_N(F("\n\rFactory Reset Cancelled"));
    };

    void update_firmware_SD(void)
    {

        // Testing 
        getFirmwareFilename();
    }

public:
    flxSysFirmware() : _pSerialSettings{nullptr}, _fileSystem{nullptr}
    {

        // Set name and description
        setName("System", "Reset and Update Options");

        flxRegister(factoryReset, "Factory Reset", "Factory reset the device - enter 1 to perform the reset");
        flxRegister(updateFirmwareSD, "Update Firmware - SD Card", "Update the firmware from the SD card");

        flxRegister(updateFirmwareFile, "Firmware Filename", "Filename to use for firmware updates");

    }
    
    void setSerialSettings(flxSettingsSerial *pSettings)
    {
        _pSerialSettings=pSettings;
    }
    void setSerialSettings(flxSettingsSerial &serSettings)
    {
        setSerialSettings(&serSettings);
    }

    //---------------------------------------------------------
    void setFileSystem(flxIFileSystem *fs)
    {
        _fileSystem = fs;
    }

    // Our input parameters/functions
    flxParameterInBool<flxSysFirmware, &flxSysFirmware::factory_reset> factoryReset;

    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::update_firmware_SD> updateFirmwareSD;

private:
    // A property that contains the name of the update firmware file
    flxPropertyHiddenString<flxSysFirmware> updateFirmwareFile;

    flxSettingsSerial *_pSerialSettings;

    // Filesystem to load a file from
    flxIFileSystem *_fileSystem;
    
};