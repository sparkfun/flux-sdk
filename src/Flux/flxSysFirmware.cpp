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

#include "flxSysFirmware.h"

#include <Update.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <nvs_flash.h>

#define kFirmwareFileExtension "bin"

#define kFirmwareUpdatePageSize  2048

const char chCR = 13; // for display erase during progress

//-----------------------------------------------------------------------------------
// factoryReset()
//
// Sets the system back to the *factory* firmware and erases nvs
void flxSysFirmware::doFactoryReset(void)
{

    flxLog_N("\n\r");
    flxLog_I_("Performing Factory Reset...");

    // do we have a factory partition on the device?
    esp_partition_iterator_t par_it =
        esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

    // Factory partition exist?
    if (par_it == NULL)
    {
        flxLog_N(F("Failed. Factory Installed firmware not installed."));
        return;
    }
    flxLog_N_(".");

    const esp_partition_t *factory = esp_partition_get(par_it);
    esp_partition_iterator_release(par_it);
    esp_err_t err = esp_ota_set_boot_partition(factory);

    flxLog_N_(".");

    if (err != ESP_OK)
    {
        flxLog_N(F("Failed. Error setting factory firmware bootable."));
        return;
    }
    // Firmware boot set to the factory partition, now reset the NVS

    // call low level ESP IDF functions to do this - as recommended by ESP32 docs ...
    nvs_flash_erase();
    flxLog_N_(".");
    nvs_flash_init();

    // Now
    flxLog_N(F("complete - rebooting..."));

    delay(500);

    esp_restart();
}

//-----------------------------------------------------------------------------------
// make sure we have OTA partitions
bool flxSysFirmware::verifyBoardOTASupport(void)
{
    // Let's make sure we have sufficient OTA partitions to support an update. We need at least two
    // OTA partitions

    int nOTA = 0;
    esp_partition_iterator_t it;

    for (uint i; i < ESP_PARTITION_SUBTYPE_APP_OTA_MAX - ESP_PARTITION_SUBTYPE_APP_OTA_MIN; i++)
    {
        it = esp_partition_find(ESP_PARTITION_TYPE_APP,
                                (esp_partition_subtype_t)(ESP_PARTITION_SUBTYPE_APP_OTA_MIN + i), nullptr);

        if (it != nullptr)
        {
            nOTA++;
            if (nOTA > 1)
                break;
        }
    }

    if (nOTA < 2)
    {
        flxLog_E(F("Invalid partition table on device - unable to update firmware."));
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------------
// Routine to pull data from stream and update OTA
//

bool flxSysFirmware::writeOTAUpdateFromStream(Stream *fFirmware, size_t updateSize)
{

    if (!fFirmware || !updateSize)
        return false;

    // Crank up the Update system
    if (!Update.begin(updateSize))
    {
        flxLog_E(F("Firmware update startup failed to begin."));
        return false;
    }

    byte dataArray[kFirmwareUpdatePageSize];
    uint bytesWritten = 0;

    // update loop
    size_t bytesToWrite;
    int barWidth = 20;
    int displayPercent = 0;
    int percentWritten = 0;

    flxLog_N_(F("Updating firmware... (00%%)"));

    while (bytesWritten < updateSize)
    {
        bytesToWrite = fFirmware->available();

        if (!bytesToWrite)
            break;

        if (bytesToWrite > kFirmwareUpdatePageSize)
            bytesToWrite = kFirmwareUpdatePageSize;

        fFirmware->readBytes(dataArray, bytesToWrite);

        if (Update.write(dataArray, bytesToWrite) != bytesToWrite)
        {
            flxLog_E(F("Error writing firmware to device. Binary might be incorrectly aligned."));
            break;
        }
        bytesWritten += bytesToWrite;

        percentWritten = (bytesWritten * 100) / updateSize;
        if (percentWritten > displayPercent)
        {
            displayPercent = percentWritten;
            Serial.write(&chCR, 1);
            flxLog_N_("Updating firmware... (%2d%%)", displayPercent);
        }
    }

    flxLog_N("");

    if (Update.end())
    {
        if (Update.isFinished())
            return true;
    }
    else
        flxLog_E("Update error: %s", Update.errorString());    

    return false;
}

//-----------------------------------------------------------------------------------
// Update Firmware from SD card section
//-----------------------------------------------------------------------------------
int flxSysFirmware::getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit)
{

    if (!_fileSystem)
    {
        flxLog_E(F("No filesystem available."));
        return 0;
    }

    flxFSFile dirRoot = _fileSystem->open("/", flxIFileSystem::kFileRead, false);

    if (!dirRoot || !dirRoot.isDirectory())
    {
        flxLog_E(F("Error accessing SD Card"));
        return 0;
    }

    std::string filename;
    size_t dot;

    std::string blank = "";

    int nFound = 0;

    while (true)
    {

        filename = dirRoot.getNextFilename();

        // empty name == done
        if (filename.length() == 0)
            break;

        dot = filename.find_last_of(".");
        if (dot == std::string::npos)
            continue; // no file extension

        if (filename.compare(dot + 1, strlen(kFirmwareFileExtension), kFirmwareFileExtension))
            continue;

        // We have a bin file, how about a firmware file - check against our prefix if one set
        // Note adding one to filename  - it always starts with "/"

        if (_firmwareFilePrefix.length() > 0 &&
            strncmp(_firmwareFilePrefix.c_str(), filename.c_str() + 1, _firmwareFilePrefix.length()) != 0)
            continue; // no match

        // We have a match
        dataLimit.addItem(blank, filename);
        nFound++;
    }

    return nFound;
}
//-----------------------------------------------------------------------------------
// getFirmwareFilename()

// Internal method - called to build up a menu, and use the serial settings system to
// allow the user to select a firmware file that is on the SD card.

bool flxSysFirmware::getFirmwareFilename(void)
{

    flxDataLimitSetString dataLimit;

    // TODO: Loop over the files on the SD  card, find the firmware files and add them to
    // the limit set.

    int nFound = getFirmwareFilesFromSD(dataLimit);

    if (nFound == 0)
    {
        flxLog_I(F("No firmware files found on SD card. File naming pattern: %s*.%s"), _firmwareFilePrefix.c_str(),
                 kFirmwareFileExtension);
        return false;
    }
    // Set the limit on our Filename property

    updateFirmwareFile.setDataLimit(dataLimit);

    // This is a *hack* to enable interactive UX for the selection of a file to use ...

    if (!_pSerialSettings)
    {
        flxLog_E(F("No Settings interface available."));
        return false;
    }
    bool status = _pSerialSettings->drawPage(this, &updateFirmwareFile);

    if (status)
    {
        flxLog_I(F("Update File is: %s"), updateFirmwareFile.get().c_str());
    }
    else
        flxLog_E(F("Update File not selected"));

    return status;
}

//-----------------------------------------------------------------------------------
bool flxSysFirmware::updateFirmwareFromSD()
{

    // Have the user select a file
    if (!getFirmwareFilename())
        return false;

    // double check that the file exists

    if (!_fileSystem->exists(updateFirmwareFile.get().c_str()))
    {
        flxLog_E(F("The firmware file, %s, does not exist on the SD card."), updateFirmwareFile.get().c_str());
        return false;
    }

    // Is the board partitions setup to support OTA?
    if (!verifyBoardOTASupport())
        return false;

    flxFSFile fFirmware = _fileSystem->open(updateFirmwareFile.get().c_str(), flxIFileSystem::kFileRead);

    if (!fFirmware)
    {
        flxLog_E(F("Error opening firmware file: %s"), updateFirmwareFile.get().c_str());
        return false;
    }

    size_t updateSize = fFirmware.size();

    if (updateSize == 0)
    {
        flxLog_E(F("Firmware file is empty: %s"), updateFirmwareFile.get().c_str());
        fFirmware.close();
        return false;
    }

    bool bStatus = writeOTAUpdateFromStream(fFirmware.stream(), updateSize);

    fFirmware.close();
    if (!bStatus)
    {
        flxLog_E(F("Firmware update failed. Please try again."));
        return false;
    }
    flxLog_I("Firmware update completed successfully. Rebooting...");
    delay(1000);
    esp_restart();

    return true;
}

//-----------------------------------------------------------------------------------
// OTA Things
//-----------------------------------------------------------------------------------

bool flxSysFirmware::getOTAFirmwareManifest(JsonDocument &jsonDoc)
{
    if (!_otaURL)
        return false;

    // get the json document 
    HTTPClient http;
        
    http.begin(_otaURL);

    if (http.GET() != HTTP_CODE_OK)
    {
        flxLog_E(F("Unable to access update manifest from server"));
        http.end();
        return false;
    }
    
    String payload = http.getString();

    http.end();

    if (deserializeJson(jsonDoc, payload.c_str()) != DeserializationError::Ok)
    {
        flxLog_E(F("Invalid update manifest file."));
        return false;

    }

    return true;
}



//-----------------------------------------------------------------------------------------------------
//
bool flxSysFirmware::writeOTAUpdateFromWiFi(WiFiClient *fFirmware, size_t updateSize, const char * md5Firmware)
{

    if (!fFirmware || !updateSize)
        return false;

    // Crank up the Update system
    if (!Update.begin(updateSize))
    {
        flxLog_E(F("Firmware update startup failed to begin."));
        return false;
    }
    // if we have an MD5 string, set it - the Update system will verify the binary - note set after begin is called
    if (md5Firmware){

        if ( !Update.setMD5(md5Firmware) )
        {
            flxLog_E(F("Unable to verify firmware contents"));
            return false;
        }
    }

    byte dataArray[kFirmwareUpdatePageSize];
    uint bytesWritten = 0;

    // update loop setup

    size_t bytesToWrite;
    int barWidth = 20;
    int displayPercent = 0;
    int percentWritten = 0;

    flxLog_N_(F("\tUpdating firmware... (00%%)"));

    // check connected status -- wifi connections can be slow/spurty...
    while (fFirmware->connected() && bytesWritten < updateSize)
    {
        bytesToWrite = fFirmware->available();

        if (!bytesToWrite)
        {
            delay(200);
            continue;
        }

        if (bytesToWrite > kFirmwareUpdatePageSize)
            bytesToWrite = kFirmwareUpdatePageSize;

        fFirmware->readBytes(dataArray, bytesToWrite);

        if (Update.write(dataArray, bytesToWrite) != bytesToWrite)
        {
            flxLog_E(F("Error writing firmware to device. Binary might be incorrectly aligned."));
            break;
        }
        bytesWritten += bytesToWrite;

        percentWritten = (bytesWritten * 100) / updateSize;
        if (percentWritten > displayPercent)
        {
            displayPercent = percentWritten;
            Serial.write(&chCR, 1);
            flxLog_N_("\tUpdating firmware... (%2d%%)", displayPercent);
        }

    }

    flxLog_N(""); // end the updat
    if (Update.end())
    {
        if (Update.isFinished())
            return true;
    }
    else
        flxLog_E("Update error: %s", Update.errorString());

    return false;
}
//-----------------------------------------------------------------------------------
bool flxSysFirmware::doFirmwareUpdateFromOTA(const char *firmwareURL, const char *md5)
{

    if (!firmwareURL)
    {
        flxLog_E(F("Invalid firmware URL"));
        return false;
    }

    HTTPClient http;
    http.begin(firmwareURL);

    int ret = http.GET();

    if (ret != HTTP_CODE_OK)
    {
        http.end();
        flxLog_E(F("Error accessing update firmware file. Error %d"), ret);
        return false;
    }

    size_t updateSize = http.getSize();

    bool bStatus = writeOTAUpdateFromWiFi(http.getStreamPtr(), updateSize, md5);

    http.end();
    if (!bStatus)
    {
        flxLog_E(F("Firmware update failed. Please try again."));
        return false;
    }
    flxLog_I("Firmware update completed successfully. Rebooting...");
    delay(1000);
    esp_restart();

    return true;
}



bool flxSysFirmware::updateFirmwareFromOTA(void)
{
    // do we have WiFi set?

    if (!_wifiConnection || !_wifiConnection->isConnected())
    {
        flxLog_E(F("Unable to check for firmware updates - no WiFi connection"));
        return false;
    }

    // update URL?
    if (!_otaURL)
    {
        flxLog_E(F("Unable to check for firmware updates - update URL not configured"));
        return false;
    }

    flxLog_N_("\tChecking for available firmware update ...");

    StaticJsonDocument<2000> _updateManifest;

    if (!getOTAFirmwareManifest(_updateManifest))
        return false;

    flxLog_N_(".");

    if (!_updateManifest.containsKey("firmware"))
    {
        flxLog_E(F("Invalid update manifest recieved - unable to continue"));
        return false;
    }

    JsonObject theEntry;
    const char * appClassID = flux.appClassID();

    if (!appClassID)
    {
        flxLog_E(F("Application Name class not set - unable to search for firmware"));
        return false;
    }

    uint32_t appVersion = flux.version();

    for (auto firmwareEntry : _updateManifest["firmware"].as<JsonArray>())
    {
        // ID? 
        if (!firmwareEntry.containsKey("ID"))
            continue;

        if ( strcmp(firmwareEntry["ID"].as<const char*>(), appClassID) != 0)
            continue;

        if (firmwareEntry["VersionNumber"].as<unsigned long>() > appVersion)
        {
            theEntry = firmwareEntry;
            break;
        }
        // if we are here, we have an entry.

    }

    if (theEntry.isNull())
    {
        Serial.printf("no updates available\n\r");
        return true;
    }

    flxLog_N(".");
    // Is this entry of a higher version? Need better checks ...version, hash, url
    //TODO

    if (!_pSerialSettings)
    {
        flxLog_E(F("No Settings interface available."));
        return false;
    }

    char szVersion[64];
    flux.versionString(szVersion, sizeof(szVersion));
    // Need to prompt for an a-okay ...
    Serial.printf("\n\r\tUpdate firmware from `%s` to version `%s` [Y/n]? ", szVersion, theEntry["Version"].as<const char*>());

    uint8_t selected = _pSerialSettings->getMenuSelectionYN();

    Serial.printf("\n\r\n\r");
    if (selected != 'y' || selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
    {
        Serial.printf("\tAborting update\n\r\r");
        return false;
    }

    // go time
    return doFirmwareUpdateFromOTA(theEntry["URL"].as<const char*>(), theEntry["Hash"].as<const char*>());


}