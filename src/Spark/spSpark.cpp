

#include <Arduino.h>
#include <ArduinoJson.h>

#include "spSpark.h"
#include "spStorage.h"

// Global object - for quick access to Spark.
spSpark& spark = spSpark::get();
//-------------------------------------------------------
//
// Note: Autoload is true by default
bool spSpark::start(bool bAutoLoad)
{

    // Init our I2C driver
    _i2cDriver.begin();

    if (bAutoLoad)
    {
        // Build drivers for the registerd devices connected to the system
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

    // Actions
    spAction *pAction;
    for (int i = 0; i < Actions.size(); i++)
    {
        pAction = Actions.at(i);
        if (!pAction)
            break;
        rc = rc || pAction->loop();
    }

    // i2c devices
    for (int i = 0; i < Devices.size(); i++)
        rc = rc || Devices.at(i)->loop();

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
void spSpark::add(_spDevice *theDevice )
{
	if ( !theDevice->autoload() )
		spDeviceFactory::get().purneAutoload(*theDevice, Devices);

	Devices.add(theDevice);
}
//------------------------------------------------------------------------------
// serializeJSON()
//
// Used to get JSON version of the system
bool spSpark::serializeJSON(char *szBuffer, size_t sz)
{

   // StaticJsonDocument<500> jDoc;

   // Actions.serializeJSON(jDoc);
   // Devices.serializeJSON(jDoc);

   // serializeJson(jDoc, szBuffer, sz);

    return true;
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