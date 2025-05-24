/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

// Action to mange the device/system Firmware - perform a factory reset, apply firmware updates

#pragma once

#include "flxCore.h"
#include "flxFS.h"
#include "flxFlux.h"
#include "flxSettingsSerial.h"

// for OTA
#include <Arduino.h>
#include <ArduinoJson.h>

// Define the firmware load event
flxDefineEventID(kOnFirmwareLoad);

class flxSysFirmware : public flxActionType<flxSysFirmware>
{

  private:
    bool doFactoryReset(void);
    bool verifyBoardOTASupport(void);
    bool writeOTAUpdateFromStream(Stream *, size_t);
    // bool writeOTAUpdateFromWiFi(WiFiClient *client, size_t size, const char *md5 = nullptr);
    bool getFirmwareFilename(void);
    bool updateFirmwareFromSD(void);
    // bool updateFirmwareFromOTA(void);

    //------------------------------------------------------------------------------

    // void factory_reset(void)
    // {
    //     bool status = factoryResetDevice();
    // };

    void update_firmware_SD(void)
    {

        bool status = updateFirmwareFromSD();
    }

    // void update_firmware_OTA(void)
    // {
    //     bool status = updateFirmwareFromOTA();
    // }

  public:
    flxSysFirmware() : _pSerialSettings{nullptr}, _fileSystem{nullptr}, _firmwareFilePrefix{""}

    {

        // Set name and description
        setName("System Update", "Firmware Update Options");

        flxRegister(updateFirmwareSD, "Update Firmware - SD Card", "Update the firmware from the SD card");
        updateFirmwareSD.prompt = false;

        flxRegister(updateFirmwareFile, "Firmware Filename", "Filename to use for firmware updates");
    }

    void setSerialSettings(flxSettingsSerial *pSettings)
    {
        _pSerialSettings = pSettings;
    }
    void setSerialSettings(flxSettingsSerial &serSettings)
    {
        setSerialSettings(&serSettings);
    }

    //---------------------------------------------------------
    void setFirmwareFilePrefix(const char *prefix)
    {
        if (prefix && strlen(prefix) > 5)
            _firmwareFilePrefix = prefix;
    }

    const char *firmwareFilePrefix(void)
    {
        return _firmwareFilePrefix.c_str();
    }
    //---------------------------------------------------------
    void setFileSystem(flxIFileSystem *fs)
    {
        _fileSystem = fs;
    }

    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::update_firmware_SD> updateFirmwareSD;

  private:
    int getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit);

    // A property that contains the name of the update firmware file
    flxPropertyHiddenString<flxSysFirmware> updateFirmwareFile;

    flxSettingsSerial *_pSerialSettings;

    // Filesystem to load a file from
    flxIFileSystem *_fileSystem;

    std::string _firmwareFilePrefix;
};
