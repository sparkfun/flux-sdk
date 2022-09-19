/*
 *
 * QwiicDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include <SparkFunBME280.h>

// What is the name used to ID this device?
#define kBME280DeviceName "bme280";
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBME280 : public spDevice<spDevBME280>, public BME280
{

  public:
    spDevBME280();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBME280DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // Called when a managed property is updated
    void onPropertyUpdate(const char *);

    // Define our public/managed properites for this class.
    // These same properties are registered with the system in the object constructor
    spPropertyBool celsius;

    // output args
    spParamOutFlt temperature_f;
    spParamOutFlt temperature_c;
    spParamOutFlt humidity;
    spParamOutFlt pressure;

    // Type testing:
    // A static instance var - that is an object (can check instance pointer)
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }
};
