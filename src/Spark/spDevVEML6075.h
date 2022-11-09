/*
 *
 *  spDevVEML6075.h
 *
 *  Spark Device object for the VEML6075 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_VEML6075_Arduino_Library.h"

// What is the name used to ID this device?
#define kVEML6075DeviceName "VEML6075"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevVEML6075 : public spDeviceI2CType<spDevVEML6075>, public VEML6075
{

public:
    spDevVEML6075();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kVEML6075DeviceName;
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
    float read_uva();
    float read_uvb();
    float read_uv_index();

    // methods to get/set our read-write properties
    uint8_t get_integration_time();
    void set_integration_time(uint8_t);
    uint8_t get_high_dynamic();
    void set_high_dynamic(uint8_t);

    uint8_t _integrationTime = VEML6075::IT_100MS;
    uint8_t _highDynamic = VEML6075::DYNAMIC_NORMAL;

    bool _begun = false;

public:
    // Define our read-write properties
    spPropertyRWUint8<spDevVEML6075, &spDevVEML6075::get_integration_time, &spDevVEML6075::set_integration_time> integrationTime
        = { VEML6075::IT_100MS, { { "50ms", VEML6075::IT_50MS }, { "100ms", VEML6075::IT_100MS },
                                  { "200ms", VEML6075::IT_200MS }, { "400ms", VEML6075::IT_400MS }, { "800ms", VEML6075::IT_800MS } } };
    spPropertyRWUint8<spDevVEML6075, &spDevVEML6075::get_high_dynamic, &spDevVEML6075::set_high_dynamic> highDynamic
        = { VEML6075::DYNAMIC_NORMAL, { { "Normal Dynamic", VEML6075::DYNAMIC_NORMAL }, { "High Dynamic", VEML6075::DYNAMIC_HIGH } } };

    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevVEML6075, &spDevVEML6075::read_uva> uva;    
    spParameterOutFloat<spDevVEML6075, &spDevVEML6075::read_uvb> uvb;    
    spParameterOutFloat<spDevVEML6075, &spDevVEML6075::read_uv_index> uvIndex;    
};
