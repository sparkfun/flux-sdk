/*
 *
 *  spDevFS3000.h
 *
 *  Spark Device object for the FS3000 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_FS3000_Arduino_Library.h"
#include "spDevice.h"



#define kFS3000DeviceName "fs3000"

// Define our class
class spDevFS3000 : public spDeviceType<spDevFS3000>, public FS3000
{

public:
    spDevFS3000();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kFS3000DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_mps();
    float read_mph();

    bool _fs3000_1005 = true;

    uint8_t get_fs3000_version();
    void set_fs3000_version(uint8_t);

    bool _begun = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevFS3000, &spDevFS3000::read_mps> flow_mps;
    spParameterOutFloat<spDevFS3000, &spDevFS3000::read_mph> flow_mph;

    spPropertyRWUint8<spDevFS3000, &spDevFS3000::get_fs3000_version, &spDevFS3000::set_fs3000_version> fs3000version
         = { 1 , { { "FS3000_1005", 1 }, { "FS3000_1015", 0 } } };
};
