/*
 *
 * spDevCCS811.h
 *
 *  Spark Device object for the CCS811 Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFunCCS811.h"
#include "spDevice.h"



#define kCCS811DeviceName "ccs811";

// Define our class
class spDevCCS811 : public spDevice<spDevCCS811>, public CCS811
{

  public:
    spDevCCS811();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kCCS811DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    float getCO2();
    float getTVOC();

    spParamOutFlt co2;
    spParamOutFlt tvoc;

    // Type testing:
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }
};
