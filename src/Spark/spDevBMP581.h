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
#define kBMP581DeviceName "bmp581"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBMP581 : public spDeviceType<spDevBMP581>, public BMP581
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
	bmp5_sensor_data bmpData;

    // methods used to get values for our output parameters
    float read_TemperatureC();
    float read_Pressure();

    // flags to prevent getSensorData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevBMP581, &spDevBMP581::read_TemperatureC> temperatureC;
    spParameterOutFloat<spDevBMP581, &spDevBMP581::read_Pressure> pressure;

};
