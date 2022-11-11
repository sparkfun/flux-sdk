

#include <Arduino.h>

#include "spSerial.h"
#include "spSpark.h"
#include "spSettings.h"

// for logging - define output driver on the stack

static spLoggingDrvDefault _logDriver;

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
    spLog.setLogLevel(spLogInfo);  // TODO - adjust?

    // Init our I2C driver
    _i2cDriver.begin();

    // Init our SPI driver
    _spiDriver.begin(true);

    if (bAutoLoad)
    {
        // Build drivers for the registered devices connected to the system
        spDeviceFactory::get().buildDevices(_i2cDriver);

        // restore state - loads save property values for this object and
        // connected devices.
        // 6/10 TODO - Something is broken with restore and container crap
        // restore();
    }

    // Everything should be loaded -- restore settings from storage
    if ( spSettings.isAvailable())
    {
        spLog_I(F("Restoring System Settings ..."));
        if (!spSettings.restoreSystem())
            spLog_W(F("Error encountered restoring system settings..."));
    }else
        spLog_I(F("Restore of System Settings unavailable."));

    // initialize actions

    for ( auto pAction : Actions )
    {
        if ( !pAction->initialize())
            spLog_W(F("Startup - Action %s initialize failed."), pAction->name());
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

// functions for external access - lifecycle things.
// These are syntactically easier to call (I think) from a user standpont.

bool spark_start(bool bAutoLoad)
{

    return spSpark::get().start(bAutoLoad);
}
bool spark_loop()
{

    return spSpark::get().loop();
}