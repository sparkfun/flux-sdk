/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevSCD30.h
 *
 *  Spark Device object for the SCD30 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_SCD30_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kSCD30DeviceName "SCD30"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevSCD30 : public flxDeviceI2CType<flxDevSCD30>, public SCD30
{

  public:
    flxDevSCD30();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kSCD30DeviceName;
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
    uint32_t read_CO2();
    float read_temperature_C();
    float read_humidity();

    // methods used to get values for our RW properties
    uint32_t get_measurement_interval();
    uint32_t get_altitude_compensation();
    float get_temperature_offset();
    void set_measurement_interval(uint32_t);
    void set_altitude_compensation(uint32_t);
    void set_temperature_offset(float);

  public:
    flxPropertyRWUInt32<flxDevSCD30, &flxDevSCD30::get_measurement_interval, &flxDevSCD30::set_measurement_interval>
        measurementInterval;
    flxPropertyRWUInt32<flxDevSCD30, &flxDevSCD30::get_altitude_compensation, &flxDevSCD30::set_altitude_compensation>
        altitudeCompensation;
    // Temp offset is only positive. See:
    // https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library/issues/27#issuecomment-971986826
    //"The SCD30 offset temperature is obtained by subtracting the reference temperature from the SCD30 output
    // temperature"
    // https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/9.5_CO2/Sensirion_CO2_Sensors_SCD30_Low_Power_Mode.pdf
    flxPropertyRWFloat<flxDevSCD30, &flxDevSCD30::get_temperature_offset, &flxDevSCD30::set_temperature_offset>
        temperatureOffset;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt32<flxDevSCD30, &flxDevSCD30::read_CO2> co2PPM;
    flxParameterOutFloat<flxDevSCD30, &flxDevSCD30::read_temperature_C> temperatureC;
    flxParameterOutFloat<flxDevSCD30, &flxDevSCD30::read_humidity> humidity;
};
