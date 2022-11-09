/*
 *
 *  spDevLPS25HB.h
 *
 *  Spark Device object for the LPS25HB device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_LPS25HB_Arduino_Library.h"
#include "spDevice.h"

#define kLPS25HBDeviceName "LPS25HB"

// Define our class
class spDevLPS25HB : public spDeviceI2CType<spDevLPS25HB>, public LPS25HB
{

  public:
    spDevLPS25HB();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kLPS25HBDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_temperature_c();
    float read_pressure_hpa();

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevLPS25HB, &spDevLPS25HB::read_temperature_c> temperatureC;
    spParameterOutFloat<spDevLPS25HB, &spDevLPS25HB::read_pressure_hpa> pressurehPa;    
};
