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


#include <esp_partition.h>
#include <esp_ota_ops.h>
#include <nvs_flash.h>

#define kFirmwareFileExtension "bin"

//-----------------------------------------------------------------------------------
// factoryReset()
//
// Sets the system back to the *factory* firmware and erases nvs
void flxSysFirmware::doFactoryReset(void)
{

    flxLog_N("\n\r");
    flxLog_I_("Performing Factory Reset...");

    // do we have a factory partition on the device?
    esp_partition_iterator_t par_it = esp_partition_find(
            ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

    // Factory partition exist?
    if (par_it == NULL)
    {
        flxLog_N(F("Failed. Factory Installed firmware not installed."));
        return;
    }
    flxLog_N_(".");

    const esp_partition_t *factory = esp_partition_get(par_it);
    esp_partition_iterator_release(par_it);
    esp_err_t  err = esp_ota_set_boot_partition(factory);

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

int flxSysFirmware::getFirmwareFilesFromSD(flxDataLimitSetString &dataLimit)
{

    if ( !_fileSystem)
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

    int nFound=0;

    while(true)
    {

        filename = dirRoot.getNextFilename();

        // empty name == done
        if (filename.length() == 0)
            break;

        dot = filename.find_last_of(".");
        if (dot == std::string::npos)
            continue; // no file extension

        if (filename.compare(dot+1, strlen(kFirmwareFileExtension), kFirmwareFileExtension))
            continue;

        // We have a bin file, how about a firmware file - check against our prefix if one set
        // Note adding one to filename  - it always starts with "/"
        
        if (_firmwareFilePrefix.length() > 0 &&  
                strncmp(_firmwareFilePrefix.c_str(), filename.c_str()+1, _firmwareFilePrefix.length()) != 0)
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

    int nFound= getFirmwareFilesFromSD(dataLimit);

    if (nFound == 0 )
    {
        flxLog_I(F("No firmware files found on SD card. File naming pattern: %s*.%s"),
                 _firmwareFilePrefix.c_str(), kFirmwareFileExtension);
        return false;
    }
    // Set the limit on our Filename property

    updateFirmwareFile.setDataLimit(dataLimit);

    // This is a *hack* to enable interactive UX for the selection of a file to use ...

    if ( !_pSerialSettings)
    {
        flxLog_E(F("No Settings interface available."));
        return false;
    }
    bool status =  _pSerialSettings->drawPage(this, &updateFirmwareFile);

    if (status)
    {
        flxLog_I(F("Update File is: %s"), updateFirmwareFile.get().c_str());
    }else
        flxLog_E(F("Update File not selected"));


    return status;


}
//-----------------------------------------------------------------------------------
// updateFirmwareFromSD()