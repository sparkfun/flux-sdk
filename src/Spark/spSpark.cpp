

#include <Arduino.h>

#include "spSerial.h"
#include "spSettings.h"
#include "spSpark.h"
#include "flxStorage.h"

// for logging - define output driver on the stack

static spLoggingDrvDefault _logDriver;

#define kApplicationHashIDTag "Application ID"

// Global object - for quick access to Spark.
spSpark &spark = spSpark::get();
//-------------------------------------------------------
//
// Note: Autoload is true by default
bool spSpark::start(bool bAutoLoad)
{

    // setup our logging system.
    _logDriver.setOutput(spSerial());
    spLog.setLogDriver(_logDriver);
    spLog.setLogLevel(spLogInfo); // TODO - adjust?



    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->setup())
        {
            spLog_E(F("Error during application setup"));
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
        spDeviceFactory::get().buildDevices(i2cDriver());

    // Everything should be loaded -- restore settings from storage
    if (spSettings.isAvailable())
    {
        spLog_I_(F("Restoring System Settings ..."));
        if (!spSettings.restoreSystem())
            spLog_W(F("Error encountered restoring system settings..."));
    }
    else
        spLog_I(F("Restore of System Settings unavailable."));

    // initialize actions
    for (auto pAction : Actions)
    {
        if (!pAction->initialize())
            spLog_W(F("[Startup] %s failed to initialize."), pAction->name());
    }

    // Call start on the application
    // Loop in the application
    if (_theApplication)
    {
        if (!_theApplication->start())
        {
            spLog_E(F("Error during application start"));
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
bool spSpark::loop(void)
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
// To pevent this, if a device added that is not autoload, we have the
// device list checked and pruned!
void spSpark::add(spDevice *theDevice)
{
    if (!theDevice->autoload())
        spDeviceFactory::get().purneAutoload(theDevice, Devices);

    Devices.push_back(theDevice);
}

#define kApplicationHashIDSize 24

//---------------------------------------------------------------------------------
bool spSpark::save(flxStorage *pStorage)
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

        status = sp_utils::id_hash_string_to_string(szBuffer, szHash, sizeof(szHash));

        // Write out the ID tag
        if (status)
            status = stBlk->write(kApplicationHashIDTag, szHash);

        pStorage->endBlock(stBlk);
    }else 
        status = true;   // external storage - so continue with save ...

    // everything go okay?
    if (!status)
    {
        spLog_D(F("Unable to store application ID key"));
        return false;
    }

    // call super class
    return spObjectContainer::save(pStorage);
};

//---------------------------------------------------------------------------------
bool spSpark::restore(flxStorage *pStorage)
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
    // source name. This makes it easlier to manually write out a settings file
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

            status = sp_utils::id_hash_string_to_string(szBuffer, szHash, sizeof(szHash));

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
        spLog_D(F("System settings not available for restoration from %s"), pStorage->name());
        return false;
    }
    // call superclass
    return spObjectContainer::restore(pStorage);
}
