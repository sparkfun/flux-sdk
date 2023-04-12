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
#include "flxFS.h"
#include "flxFlux.h"
#include "flxSettingsSerial.h"

// for OTA
#include "ESP32OTAPull.h"
#include "flxWiFiESP32.h"

class flxSysFirmware : public flxActionType<flxSysFirmware>
{

  private:
    void doFactoryReset(void);
    bool verifyBoardOTASupport(void);
    bool writeOTAUpdateFromStream(Stream *, size_t);
    bool getFirmwareFilename(void);
    bool updateFirmwareFromSD(void);
    bool updateFirmwareFromOTA(void);

    // Experiement with OTA option

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

        bool status = updateFirmwareFromSD();
    }

    void update_firmware_OTA(void)
    {
        bool status = updateFirmwareFromOTA();
    }

  public:
    flxSysFirmware()
        : _pSerialSettings{nullptr}, _fileSystem{nullptr}, _firmwareFilePrefix{""},
          _wifiConnection{nullptr}, _otaURL{nullptr}, _bUpdateOTA{false}
    {

        // Set name and description
        setName("System Update", "Device Reset and Firmware Update Options");

        flxRegister(factoryReset, "Factory Reset", "Factory reset the device - enter 1 to perform the reset");

        flxRegister(updateFirmwareSD, "Update Firmware - SD Card", "Update the firmware from the SD card");
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

    // Our input parameters/functions
    flxParameterInBool<flxSysFirmware, &flxSysFirmware::factory_reset> factoryReset;

    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::update_firmware_SD> updateFirmwareSD;

    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::update_firmware_OTA> updateFirmwareOTA;

    // for OTA
    void setWiFiDevice(flxWiFiESP32 *pWiFi)
    {
        _wifiConnection = pWiFi;
    }

    void enableOTAUpdates(const char *otaURL)
    {
        if (!otaURL)
            return;
        _otaURL = otaURL;

        if (_bUpdateOTA)
        {
            flxRegister(updateFirmwareOTA, "Update Firmware - OTA", "Update the firmware over-the-air");
            _bUpdateOTA = true;
        }
    }

  private:
    int getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit);

    bool doWiFiOTA(ESP32OTAPull &otaPull, char *currentVersion);

    // A property that contains the name of the update firmware file
    flxPropertyHiddenString<flxSysFirmware> updateFirmwareFile;

    flxSettingsSerial *_pSerialSettings;

    // Filesystem to load a file from
    flxIFileSystem *_fileSystem;

    std::string _firmwareFilePrefix;

    flxWiFiESP32 *_wifiConnection;

    const char *_otaURL;
    bool _bUpdateOTA;
};
