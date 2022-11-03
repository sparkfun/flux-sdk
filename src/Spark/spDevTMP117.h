/*
 *
 *  spDevTMP117.h
 *
 *  Spark Device object for the TMP117 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_TMP117.h"

// What is the name used to ID this device?
#define kTMP117DeviceName "TMP117"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevTMP117 : public spDeviceType<spDevTMP117>, public TMP117
{

public:
    spDevTMP117();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kTMP117DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

private:

    // methods used to get values for our output parameters
    double read_temperature_C();

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutDouble<spDevTMP117, &spDevTMP117::read_temperature_C> temperatureC;    
};
