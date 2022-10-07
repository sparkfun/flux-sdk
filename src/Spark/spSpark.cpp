

#include <Arduino.h>

#include "spSerial.h"
#include "spSpark.h"
#include "spStorage.h"

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

    // Init our I2C driver
    _i2cDriver.begin();

    if (bAutoLoad)
    {
        // Build drivers for the registered devices connected to the system
        spDeviceFactory::get().buildDevices(_i2cDriver);

        // restore state - loads save property values for this object and
        // connected devices.
        // 6/10 TODO - Something is broken with restore and container crap
        // restore();
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