/*
 *
 *  spDevAMG8833.h
 *
 *  Spark Device object for the AMG8833 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_GridEYE_Arduino_Library.h"
#include "spDevice.h"



#define kAMG8833DeviceName "AMG8833"

// Define our class
class spDevAMG8833 : public spDeviceType<spDevAMG8833>, public GridEYE
{

public:
    spDevAMG8833();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kAMG8833DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_device_temperature_C();
    bool read_pixel_temperatures(spDataArrayFloat *);

    bool _frameRate10FPS = true; // Default to 10 FPS

    uint8_t get_frame_rate();
    void set_frame_rate(uint8_t);

    bool _begun = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevAMG8833, &spDevAMG8833::read_device_temperature_C> deviceTemperatureC;
    spParameterOutArrayFloat<spDevAMG8833, &spDevAMG8833::read_pixel_temperatures> pixelTemperatures;

    spPropertyRWUint8<spDevAMG8833, &spDevAMG8833::get_frame_rate, &spDevAMG8833::set_frame_rate> frameRate
         = { 1 , { { "1 Frame Per Second", 0 }, { "10 Frames Per Second", 1 } } };
};
