/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
/*
 *
 *  flxDevSCD40.h
 *
 *  Spark Device object for the SCD40 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFun_SCD4x_Arduino_Library.h"

// What is the name used to ID this device?
#define kSCD40DeviceName "SCD40"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevSCD40 : public flxDeviceI2CType<flxDevSCD40>, public SCD4x
{

public:
    flxDevSCD40();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }
    
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
    flxPropertyRWBool<flxDevSCD40, &flxDevSCD40::get_auto_calibrate, &flxDevSCD40::set_auto_calibrate> autoCalibrate;
    // Define the sensor altitude in metres above sea level, so RH and CO2 are compensated for atmospheric pressure
    // Default altitude is 0m
    flxPropertyRWUint<flxDevSCD40, &flxDevSCD40::get_altitude_compensation, &flxDevSCD40::set_altitude_compensation> altitudeCompensation;
    // Define how warm the sensor is compared to ambient, so RH and T are temperature compensated. Has no effect on the CO2 reading
    // Default offset is 4C
    flxPropertyRWFloat<flxDevSCD40, &flxDevSCD40::get_temperature_offset, &flxDevSCD40::set_temperature_offset> temperatureOffset;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUint<flxDevSCD40, &flxDevSCD40::read_CO2> co2PPM;    
    flxParameterOutFloat<flxDevSCD40, &flxDevSCD40::read_temperature_C> temperatureC;    
    flxParameterOutFloat<flxDevSCD40, &flxDevSCD40::read_humidity> humidity;    
};
