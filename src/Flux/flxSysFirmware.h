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
#include "HTTPClient.h"
#include "flxWiFiESP32.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class flxSysFirmware : public flxActionType<flxSysFirmware>
{

  private:
    bool doFactoryReset(void);
    bool verifyBoardOTASupport(void);
    bool writeOTAUpdateFromStream(Stream *, size_t);
    bool writeOTAUpdateFromWiFi(WiFiClient *client, size_t size, const char *md5 = nullptr);
    bool getFirmwareFilename(void);
    bool updateFirmwareFromSD(void);
    bool updateFirmwareFromOTA(void);
    bool factoryResetDevice(void);

    // Experiement with OTA option

    //------------------------------------------------------------------------------

    void restartDevice(void);

    void factory_reset(void)
    {
        bool status = factoryResetDevice();
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

        flxRegister(deviceRestart, "Device Restart", "Restart/reboot the device");
        deviceRestart.prompt = false;

        flxRegister(factoryReset, "Factory Reset", "Erase all settings and revert to original firmware");
        factoryReset.prompt = false;

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

    // Our input parameters/functions
    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::restartDevice> deviceRestart;

    flxParameterInVoid<flxSysFirmware, &flxSysFirmware::factory_reset> factoryReset;

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

        if (!_bUpdateOTA)
        {
            flxRegister(updateFirmwareOTA, "Update Firmware - OTA", "Update the firmware over-the-air");
            updateFirmwareOTA.prompt = false;
            _bUpdateOTA = true;
        }
    }

    flxSignalBool on_firmwareload;

  private:
    int getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit);
    bool getOTAFirmwareManifest(JsonDocument &jsonDoc);
    bool doFirmwareUpdateFromOTA(const char *firmwareURL, const char *md5 = nullptr);

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
