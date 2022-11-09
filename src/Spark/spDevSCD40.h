/*
 *
 *  spDevSCD40.h
 *
 *  Spark Device object for the SCD40 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_SCD4x_Arduino_Library.h"

// What is the name used to ID this device?
#define kSCD40DeviceName "SCD40"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSCD40 : public spDeviceI2CType<spDevSCD40>, public SCD4x
{

public:
    spDevSCD40();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSCD40DeviceName;
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
    uint read_CO2();
    float read_temperature_C();
    float read_humidity();

    // methods used to get values for our RW properties
    bool get_auto_calibrate();
    uint get_altitude_compensation();
    float get_temperature_offset();
    void set_auto_calibrate(bool);
    void set_altitude_compensation(uint);
    void set_temperature_offset(float);

    // flags to avoid calling readM<easurement multiple times
    bool _co2 = false;
    bool _temp = false;
    bool _rh = false;

public:
    spPropertyRWBool<spDevSCD40, &spDevSCD40::get_auto_calibrate, &spDevSCD40::set_auto_calibrate> autoCalibrate;
    // Define the sensor altitude in metres above sea level, so RH and CO2 are compensated for atmospheric pressure
    // Default altitude is 0m
    spPropertyRWUint<spDevSCD40, &spDevSCD40::get_altitude_compensation, &spDevSCD40::set_altitude_compensation> altitudeCompensation;
    // Define how warm the sensor is compared to ambient, so RH and T are temperature compensated. Has no effect on the CO2 reading
    // Default offset is 4C
    spPropertyRWFloat<spDevSCD40, &spDevSCD40::get_temperature_offset, &spDevSCD40::set_temperature_offset> temperatureOffset;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevSCD40, &spDevSCD40::read_CO2> co2PPM;    
    spParameterOutFloat<spDevSCD40, &spDevSCD40::read_temperature_C> temperatureC;    
    spParameterOutFloat<spDevSCD40, &spDevSCD40::read_humidity> humidity;    
};
