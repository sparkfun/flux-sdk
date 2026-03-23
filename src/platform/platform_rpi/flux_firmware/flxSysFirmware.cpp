/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxSysFirmware.h"
#include "flxPlatform.h"
#include <LittleFS.h>
#include <PicoOTA.h>

// on board flash file system bounds

extern uint8_t _FS_start;
extern uint8_t _FS_end;

#define kFirmwareFileExtension "bin"

#define kFirmwareUpdatePageSize 2048

const char chCR = 13; // for display erase during progress

const char *kTempUpdateFileName = "update_ota.bin";

//-----------------------------------------------------------------------------------
// make sure we have OTA partitions
bool flxSysFirmware::verifyBoardOTASupport(void)
{
    // Was a filesystem set for the on-board flash?
    if (&_FS_end - &_FS_start <= 0)
        return false;

    // Make sure little fs is up
    if (LittleFS.begin() == false)
        return false;

    return true;
}

//-----------------------------------------------------------------------------------
// Routine to pull data from stream and update OTA
//

bool flxSysFirmware::writeOTAUpdateFromStream(Stream *fFirmware, size_t updateSize)
{

    if (!fFirmware || !updateSize)
        return false;

    byte dataArray[kFirmwareUpdatePageSize];
    uint bytesWritten = 0;

    // Get a file on the local filesystem to write the update to
    File fUpdate = LittleFS.open(kTempUpdateFileName, "w");
    if (!fUpdate)
    {
        flxLog_E(F("Unable to open temporary file for update"));
        return false;
    }
    flxLog_V(F("Opened temporary file for update: %s"), kTempUpdateFileName);

    // update loop
    size_t bytesToWrite;
    int barWidth = 20;
    int displayPercent = 0;
    int percentWritten = 0;

    flxSendEvent(flxEvent::kOnFirmwareLoad, true);

    flxLog_N_(F("Updating firmware... (00%%)"));

    while (bytesWritten < updateSize)
    {
        bytesToWrite = fFirmware->available();

        if (!bytesToWrite)
            break;

        if (bytesToWrite > kFirmwareUpdatePageSize)
            bytesToWrite = kFirmwareUpdatePageSize;

        fFirmware->readBytes(dataArray, bytesToWrite);

        if (fUpdate.write(dataArray, bytesToWrite) != bytesToWrite)
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
    flxSendEvent(flxEvent::kOnFirmwareLoad, false);

    // close the update file
    fUpdate.close();
    flxLog_V(F("Closed temporary file for update: %s"), kTempUpdateFileName);

    // Now trigger/setup the OTA
    picoOTA.begin();
    picoOTA.addFile(kTempUpdateFileName);
    picoOTA.commit();
    LittleFS.end();

    flxLog_V(F("Firmware OTA update setup completed"));

    return true;
}

//-----------------------------------------------------------------------------------
// Update Firmware from SD card section
//-----------------------------------------------------------------------------------
int flxSysFirmware::getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit)
{

    if (!_fileSystem)
    {
        flxLogM_E(kMsgErrInitialization, name(), "No filesystem");
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
            strncmp(_firmwareFilePrefix.c_str(), filename.c_str(), _firmwareFilePrefix.length()) != 0)
            continue; // no match

        flxLog_V(F("Update search - found firmware file: %s"), filename.c_str());
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
        flxLogM_E(kMsgErrInitialization, name(), "Settings Interface");
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

    flxPlatform::restart_device();

    return true;
}
