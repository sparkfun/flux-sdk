/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include <Arduino.h>

#include "flxFlux.h"
#include "flxSerial.h"
#include "flxSettings.h"
#include "flxStorage.h"

#include "flxCoreJobs.h"

#include "mbedtls/base64.h"

// for logging - define output driver on the stack

static flxLoggingDrvDefault _logDriver;

static bool _isInitalized = false;

const char *kApplicationHashIDTag = "Application ID";

// Global object - for quick access to Spark.
flxFlux &flux = flxFlux::get();

bool flxFlux::initialized()
{
    return _isInitalized;
}

void flxFlux::setInitialized(bool bInit)
{
    _isInitalized = bInit;
}
//-------------------------------------------------------
//
// Note: Auto-load is true by default
bool flxFlux::start()
{

    // settings - add to our system now that we are up and running.
    add(&flxSettings);

    // if we have an application, call the init method.
    // The intent is to give the app time to setup anything before
    // the system sets up

    if (_theApplication)
    {

        // if the app was set during startup (before main - when globals are instantiated), it
        // wasn't added to the system. So do that now.
        if (Actions.size() == 0 || Actions.at(0) != (flxAction *)_theApplication)
            Actions.insert(Actions.begin(), (flxAction *)_theApplication);

        _theApplication->onInit();
    }
    // setup our logging system.
    _logDriver.setOutput(flxSerial);
    flxLog.setLogDriver(_logDriver);
    flxLog.setLogLevel(flxLogInfo); // TODO - adjust?

    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->onSetup())
        {
            flxLog_E(F("Error during application setup"));
            return false;
        }

        if (strlen(_theApplication->name()) > 0)
            this->setName(_theApplication->name());

        if (strlen(_theApplication->description()) > 0)
            this->setDescription(_theApplication->description());
    }
    else
        flxLog_W(F("No application object set."));

    writeBanner();

    // Init the I2c bus - 3/2024 - found that the bus wasn't initialized and
    // auto-load wasn't called, wifi wouldn't connect -- related to i2c bus being *on*?

    flxBusI2C thei2cBus = i2cDriver();

    // Build drivers for the registered devices connected to the system
    if (_deviceAutoload)
        flxDeviceFactory::get().buildDevices(thei2cBus);

    if (_theApplication)
        _theApplication->onDeviceLoad();

    // initialize actions
    for (auto pAction : Actions)
    {
        if (!pAction->initialize())
            flxLog_W(F("[Startup] %s failed to initialize."), pAction->name());
    }
    // Everything should be loaded -- restore settings from storage
    if (_loadSettings && flxSettings.isAvailable())
    {
        // Let the device know we're starting up
        if (_theApplication)
            _theApplication->onRestore();

        flxLog_I_(F("Restoring System Settings ..."));
        if (!flxSettings.restoreSystem())
            flxLog_W(F("Error encountered restoring system settings..."));
    }
    else
        flxLog_I(F("Restore of System Settings unavailable."));

    // Call start on the application
    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->onStart())
        {
            flxLog_E(F("Error during application start"));
            return false;
        }
        // There is a chance, that after startup and device load, the
        // board name will change (devices are used to determine board name)
        // Soooo reset the name.
        if (strlen(_theApplication->name()) > 0)
            this->setName(_theApplication->name());
    }
    // start the job queue
    if (!flxJobQueue.start())
        flxLog_E("Job queue failed to start - unrecoverable error. ");
    // // else
    // // {
    // //     flxLog_I(F("Job queue initialized"));
    // //     flxJobQueue.dump();
    // // }
    setInitialized(true);

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

    // Just Call loop on the job queue system
    //
    bool rc = flxJobQueue.loop();

    // and the application loop handler if we have an app
    if (_theApplication)
        rc = rc || _theApplication->loop();

    return rc;
}
//------------------------------------------------------------------------------
// add()  -- a device pointer
//
// If a device is being added by the user, not auto-load, there is a chance
// that auto-load picked up the device before the user added it/created it.
// This leads to having "dups" in our connected device list.
//
// To prevent this, if a device added that is not auto-load, we have the
// device list checked and pruned!
//
void flxFlux::add(flxDevice *theDevice)
{
    if (!theDevice->autoload())
        flxDeviceFactory::get().pruneAutoload(theDevice, Devices);

    // do we want to use a verbose device name - one that includes an ID?
    // start with our app setting. ...
    bool isVerbose = _verboseDevNames;

    if (!isVerbose)
    {
        // Check for any name collisions before we add the device to the list ...
        for (auto device : Devices)
        {
            // If the same name, we need to use a verbose name with the added device.
            if (!strcmp(device->name(), theDevice->name()))
            {
                isVerbose = true;
                break;
            }
        }
    }

    // If using verbose naming , let's make a new name - 'NAME [ID]'.
    //  ID = HEX for I2c device, DEC for SPI
    if (isVerbose)
        theDevice->addAddressToName();

    Devices.push_back(theDevice);
}

#define kApplicationHashIDSize 24

//---------------------------------------------------------------------------------
bool flxFlux::save(flxStorage *pStorage)
{
    // Write a block to the storage system that has a has of or name/desc
    // Use this to validate that the settings in the storage system are ours

    // 11/2023 - originally used the name to find our start key, but names change
    //           switching to application class name - which doesn't change

    flxStorageBlock *stBlk = pStorage->beginBlock(appClassID());
    if (!stBlk)
        return false;

    bool status;

    // If the storage medium is *internal* to the system, save our app hash tag.
    // This allows rapid ID of a valid storage source
    if (pStorage->kind() == flxStorage::flxStorageKindInternal)
    {
        // 11/2023
        // we just want to write a key at this level that can be used on restore
        // to indicate this is this applications prefs/state. Just put hash of
        // the app class id in this store
        char szHash[kApplicationHashIDSize];

        status = flx_utils::id_hash_string_to_string(appClassID(), szHash, sizeof(szHash));

        // Write out the ID tag
        if (status)
            status = stBlk->write(kApplicationHashIDTag, szHash);

        pStorage->endBlock(stBlk);
    }
    else
        status = true; // external storage - so continue with save ...

    // everything go okay?
    if (!status)
    {
        flxLog_W(F("Unable to store application ID key"));
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

    // 11/2023 - originally used the name to find our start key, but names change
    //           switching to application class name - which doesn't change
    flxStorageBlock *stBlk = pStorage->beginBlock(appClassID());
    if (!stBlk)
        return false;

    // If the storage kind is *internal*, check for our app hash key. This provides
    // a quick check to validate the storage source.
    //
    // Note for external sources (files...etc), we load in and validate based on
    // source name. This makes it easier to manually write out a settings file
    bool status;
    bool oldIDFound = false;
    char szBuffer[128] = {0};
    if (pStorage->kind() == flxStorage::flxStorageKindInternal)
    {
        status = stBlk->valueExists(kApplicationHashIDTag);

        if (!status)
        {
            // 11/2023
            // The App pref block and key value doesn't exist, it could be we have an older system,
            // which used the app name() not app classID to name the block store the app ID key
            // so try the name(). Over time, re-saves of prefs will transition things to the
            // app ID, but we'll also transition below
            pStorage->endBlock(stBlk);
            stBlk = pStorage->beginBlock(name());
            if (!stBlk)
                return false;

            status = stBlk->valueExists(kApplicationHashIDTag);
            if (status)
            {
                oldIDFound = true;

                // use the original method for the hash ID gen - name and desc
                strlcpy(szBuffer, name(), sizeof(szBuffer));
                strlcat(szBuffer, description(), sizeof(szBuffer));
            }
        }
        else
            strlcpy(szBuffer, appClassID(), sizeof(szBuffer));

        if (status)
        {
            // just hash the app class id
            char szHash[kApplicationHashIDSize];

            status = flx_utils::id_hash_string_to_string(szBuffer, szHash, sizeof(szHash));

            if (status)
            {
                // okay, read in the tag, see what we find
                status = stBlk->readString(kApplicationHashIDTag, szBuffer, sizeof(szBuffer)) > 0;

                if (status)
                    status = strncmp(szHash, szBuffer, strlen(szHash)) == 0;
            }

            // 11/23
            // if we have success and we used an old key, transition to the new key
            if (status && oldIDFound)
            {
                pStorage->endBlock(stBlk);
                stBlk = pStorage->beginBlock(appClassID());
                if (stBlk)
                {
                    if (flx_utils::id_hash_string_to_string(appClassID(), szHash, sizeof(szHash)))
                    {
                        stBlk->setReadOnly(false);
                        if (!stBlk->write(kApplicationHashIDTag, szHash))
                            flxLog_W("Preferences app key failed to write");
                    }
                }
            }
        }

        pStorage->endBlock(stBlk);
    }
    else
        status = true; // restoring form an external source

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
// get the device ID
//
const char *flxFlux::deviceId(void)
{
    // ID is 16 in length, use a  C string
    static char szDeviceID[17] = {0};
    static bool bInitialized = false;
#ifdef ESP32

    if (!bInitialized)
    {
        memset(szDeviceID, '\0', sizeof(szDeviceID));
        snprintf(szDeviceID, sizeof(szDeviceID), "%4s%012llX", _v_idprefix, ESP.getEfuseMac());
        bInitialized = true;
    }
#endif
    return (const char *)szDeviceID;
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
void flxFlux::setAppToken(const uint8_t *data, size_t len)
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

//---------------------------------------------------------------------------------
// Manage the verbose name setting....
void flxFlux::setVerboseDevNames(bool bVerbose)
{
    if (_verboseDevNames == bVerbose)
        return;

    _verboseDevNames = bVerbose;
    if (bVerbose)
    {
        // Check for any name collisions before we add the device to the list ...
        for (auto device : Devices)
            device->addAddressToName();
    }
}

bool flxFlux::verboseDevNames(void)
{
    return _verboseDevNames;
}
flxFlux &flux_get()
{
    return flxFlux::get();
}
void flux_add(flxAction &theAction)
{
    flxFlux::get().add(&theAction);
}
void flux_add(flxAction *theAction)
{
    flxFlux::get().add(theAction);
}
void flux_add(flxDevice &theDevice)
{
    flxFlux::get().add(&theDevice);
}
void flux_add(flxDevice *theDevice)
{
    flxFlux::get().add(theDevice);
}
void flux_add(flxApplication &theApp)
{
    flxFlux::get().setApplication(&theApp);
}
void flux_add(flxApplication *theApp)
{
    flxFlux::get().setApplication(theApp);
}