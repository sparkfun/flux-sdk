/*
 *
 *  spDevBMP581.h
 *
 *  Device object for the BMP581 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_BMP581_Arduino_Library.h"

// What is the name used to ID this device?
#define kBMP581DeviceName "BMP581"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBMP581 : public spDeviceI2CType<spDevBMP581>, public BMP581
{

public:
    spDevBMP581();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBMP581DeviceName;
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
	bmp5_sensor_data bmpData = { 0.0, 0.0 };

    // methods used to get values for our output parameters
    float read_TemperatureC();
    float read_Pressure();

    // flags to prevent getSensorData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;

    uint8_t _powerMode = BMP5_POWERMODE_NORMAL;
    bool _begun = false;

    uint8_t get_power_mode();
    void set_power_mode(uint8_t);

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevBMP581, &spDevBMP581::read_TemperatureC> temperatureC;
    spParameterOutFloat<spDevBMP581, &spDevBMP581::read_Pressure> pressure;

    // Define our read-write properties
    // Note to self: we might need to restrict the available power modes if we can't guarantee spDevBMP581 handles
    // all possible/required changes of mode correctly...
    // From Dryw's notes: the sensor can only enter forced mode from sleep mode.
    //                    Transitions between forced and normal modes are ignored
    spPropertyRWUint8<spDevBMP581, &spDevBMP581::get_power_mode, &spDevBMP581::set_power_mode> powerMode
        = { BMP5_POWERMODE_NORMAL, { { "Standby", BMP5_POWERMODE_STANDBY }, { "Normal", BMP5_POWERMODE_NORMAL },
                                     { "Forced", BMP5_POWERMODE_FORCED }, { "Continuous", BMP5_POWERMODE_CONTINOUS },
                                     { "Deep_Standby", BMP5_POWERMODE_DEEP_STANDBY } } };

};
