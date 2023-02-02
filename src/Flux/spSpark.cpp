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
 

#include <Arduino.h>

#include "flxSerial.h"
#include "flxSettings.h"
#include "flxFlux.h"
#include "flxStorage.h"


#include "mbedtls/base64.h"

// for logging - define output driver on the stack

static flxLoggingDrvDefault _logDriver;

#define kApplicationHashIDTag "Application ID"

// Global object - for quick access to Spark.
flxFlux &flux = flxFlux::get();
//-------------------------------------------------------
//
// Note: Autoload is true by default
bool flxFlux::start(bool bAutoLoad)
{

    // setup our logging system.
    _logDriver.setOutput(flxSerial());
    flxLog.setLogDriver(_logDriver);
    flxLog.setLogLevel(flxLogInfo); // TODO - adjust?



    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->setup())
        {
            flxLog_E(F("Error during application setup"));
            return false;
        }

        if (strlen(_theApplication->name()) > 0)
            this->setName(_theApplication->name());

        if (strlen(_theApplication->description()) > 0)
            this->setDescription(_theApplication->description());            
    }

    writeBanner();

        // Build drivers for the registered devices connected to the system
    if (bAutoLoad)
        flxDeviceFactory::get().buildDevices(i2cDriver());

    // Everything should be loaded -- restore settings from storage
    if (flxSettings.isAvailable())
    {
        flxLog_I_(F("Restoring System Settings ..."));
        if (!flxSettings.restoreSystem())
            flxLog_W(F("Error encountered restoring system settings..."));
    }
    else
        flxLog_I(F("Restore of System Settings unavailable."));

    // initialize actions
    for (auto pAction : Actions)
    {
        if (!pAction->initialize())
            flxLog_W(F("[Startup] %s failed to initialize."), pAction->name());
    }

    // Call start on the application
    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->start())
        {
            flxLog_E(F("Error during application start"));
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// loop()
//
// Main executive loop for the logger. Expected to be called from "loop" of
// the overall app/firmware
//
// Returns true if an action returns true - aka did something
//
bool flxFlux::loop(void)
{

    // Pump our actions by calling there loop methods
    bool rc = false;

    bool rc2;
    // Actions
    for (auto pAction : Actions)
    {
        rc2 = pAction->loop();
        rc = rc || rc2;
    }

    // i2c devices
    for (auto pDevice : Devices)
    {
        rc2 = pDevice->loop();
        rc = rc || rc2;
    }

    return rc;
}
//------------------------------------------------------------------------------
// add()  -- a device pointer
//
// If a device is being added by the user, not autoload, there is a chance
// that autoload picked up the device before the user added it/created it.
// This leads to having "dups" in our connected device list.
//
// To prevent this, if a device added that is not autoload, we have the
// device list checked and pruned!
void flxFlux::add(flxDevice *theDevice)
{
    if (!theDevice->autoload())
        flxDeviceFactory::get().pruneAutoload(theDevice, Devices);

    Devices.push_back(theDevice);
}

#define kApplicationHashIDSize 24

//---------------------------------------------------------------------------------
bool flxFlux::save(flxStorage *pStorage)
{
    // Write a block to the storage system that has a has of or name/desc
    // Use this to validate that the settings in the storage system are ours
    flxStorageBlock *stBlk = pStorage->beginBlock(name());
    if (!stBlk)
        return false;

    bool status;

    // If the storage medium is *internal* to the system, save our app hash tag.
    // This allows rapid ID of a valid storage source
    if (pStorage->kind() == flxStorage::flxStorageKindInternal)
    {
        char szBuffer[128] = {0};
        strlcpy(szBuffer, name(), sizeof(szBuffer));
        strlcat(szBuffer, description(), sizeof(szBuffer));

        char szHash[kApplicationHashIDSize];

        status = flx_utils::id_hash_string_to_string(szBuffer, szHash, sizeof(szHash));

        // Write out the ID tag
        if (status)
            status = stBlk->write(kApplicationHashIDTag, szHash);

        pStorage->endBlock(stBlk);
    }else 
        status = true;   // external storage - so continue with save ...

    // everything go okay?
    if (!status)
    {
        flxLog_D(F("Unable to store application ID key"));
        return false;
    }

    // call super class
    return flxObjectContainer::save(pStorage);
};

//---------------------------------------------------------------------------------
bool flxFlux::restore(flxStorage *pStorage)
{
    // Do we have our ID block in storage? If not, then there's no need to continue
    // since the data isn't for this app

    flxStorageBlock *stBlk = pStorage->beginBlock(name());
    if (!stBlk)
        return false;

    // If the storage kind is *internal*, check for our app hash key. This provides
    // a quick check to validate the storage source. 
    //
    // Note for external sources (files...etc), we load in and validate based on 
    // source name. This makes it easier to manually write out a settings file
    bool status;
    if (pStorage->kind() == flxStorage::flxStorageKindInternal)
    {
        status = stBlk->valueExists(kApplicationHashIDTag);
        if (status)
        {
            char szBuffer[128] = {0};
            strlcpy(szBuffer, name(), sizeof(szBuffer));
            strlcat(szBuffer, description(), sizeof(szBuffer));

            char szHash[kApplicationHashIDSize];

            status = flx_utils::id_hash_string_to_string(szBuffer, szHash, sizeof(szHash));

            if (status)
            {
                // okay, read in the tag, see what we find
                status = stBlk->readString(kApplicationHashIDTag, szBuffer, sizeof(szBuffer)) > 0;

                if (status)
                    status = strncmp(szHash, szBuffer, strlen(szHash)) == 0;
            }
        }

        pStorage->endBlock(stBlk);
    }else
       status = true;  // restoring form an external source

    // everything go okay?
    if (!status)
    {
        flxLog_D(F("System settings not available for restoration from %s"), pStorage->name());
        return false;
    }
    // call superclass
    return flxObjectContainer::restore(pStorage);
}

//---------------------------------------------------------------------------------
//
// This expects an input array of a base64 32 byte (256 bit) token/key
//
// This is generated:
//      Gen a base 64 key  % openssl rand -base64 32
//      Convert into ascii ints in python %    data = [ord(c) for c in ss]
//      Map those numbers into a uint8_t array (i.e. uint8_t mykey[] = {...];)
//
void flxFlux::setAppToken(uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return;

    unsigned char szBuffer[len + 1];
    memcpy(szBuffer, data, len);

    // convert the token to something we can use.
    size_t outlen;
    mbedtls_base64_decode(_token, sizeof(_token), &outlen, szBuffer, len);
    _hasToken = true;
}
//---------------------------------------------------------------------------------
bool flxFlux::getAppToken(uint8_t outtok[32])
{
    if (!_hasToken)
        return false;

    memcpy(outtok, _token, 32);
    return true;
}
