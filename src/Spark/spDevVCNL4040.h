/*
 *
 *  spDevVCNL4040.h
 *
 *  Spark Device object for the VCNL4040 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_VCNL4040_Arduino_Library.h"

// What is the name used to ID this device?
#define kVCNL4040DeviceName "vcnl4040"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevVCNL4040 : public spDeviceType<spDevVCNL4040>, public VCNL4040
{

public:
    spDevVCNL4040();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kVCNL4040DeviceName;
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
    uint read_proximity();
    uint read_lux();

    // methods to get/set our read-write properties
    uint get_LED_current();
    void set_LED_current(uint);
    uint get_IR_duty_cycle();
    void set_IR_duty_cycle(uint);
    uint get_proximity_integration_time();
    void set_proximity_integration_time(uint);
    bool get_proximity_resolution();
    void set_proximity_resolution(bool);
    uint get_ambient_integration_time();
    void set_ambient_integration_time(uint);

    uint _ledCurrent = 200;
    uint _irDutyCycle = 40;
    uint _proxIntTime = 8;
    bool _proxRes = true; // Default to 16-bit
    uint _ambIntTime = 80;

public:
    // Define our read-write properties
    spPropertyRWUint<spDevVCNL4040, &spDevVCNL4040::get_LED_current, &spDevVCNL4040::set_LED_current> ledCurrent;
    spPropertyRWUint<spDevVCNL4040, &spDevVCNL4040::get_IR_duty_cycle, &spDevVCNL4040::set_IR_duty_cycle> irDutyCycle;
    spPropertyRWUint<spDevVCNL4040, &spDevVCNL4040::get_proximity_integration_time, &spDevVCNL4040::set_proximity_integration_time> proximityIntegrationTime;
    spPropertyRWBool<spDevVCNL4040, &spDevVCNL4040::get_proximity_resolution, &spDevVCNL4040::set_proximity_resolution> proximityResolution;
    spPropertyRWUint<spDevVCNL4040, &spDevVCNL4040::get_ambient_integration_time, &spDevVCNL4040::set_ambient_integration_time> ambientIntegrationTime;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevVCNL4040, &spDevVCNL4040::read_proximity> proximity;    
    spParameterOutUint<spDevVCNL4040, &spDevVCNL4040::read_lux> lux;    
};
