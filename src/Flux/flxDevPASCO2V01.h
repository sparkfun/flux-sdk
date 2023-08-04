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
 *  flxDevPASCO2V01.h
 *
 *  Spark Device object for the Infineon XENSIV PAS CO2 device.
 *
 *
 *
 */

#pragma once

#include <Arduino.h>

#include "flxDevice.h"
#include <pas-c02-ino.hpp>

// What is the name used to ID this device?
#define kPASCO2V01DeviceName "PASCO2V01"
//-----------------------------------------------------------------------------
// Define our class - note we are sub-classing from the original library
class flxDevPASCO2V01 : public flxDeviceI2CType<flxDevPASCO2V01>
{
public:
    flxDevPASCO2V01();
    ~flxDevPASCO2V01();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kPASCO2V01DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    };
    // holds the class list of possible addresses/IDs for this object
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

private:

    // Pointer to instance of PASCO2Ino
    PASCO2Ino *theSensor;

    // methods used to get values for our output parameters
    uint read_CO2();
    bool read_alarm_status();
    
    
    // methods used to get values for our RW properties
    bool get_auto_calibrate();
    uint get_pressure_reference();
    bool get_alarm_enabled();
    bool get_alarm_auto_clear();
    uint get_alarm_threshold();
    uint get_measurement_period();

    void set_auto_calibrate(bool);
    void set_pressure_reference(uint);
    void set_alarm_enabled(bool);
    void set_alarm_auto_clear(bool);
    void set_alarm_threshold(uint);
    void set_measurement_period(uint);

    bool _autoCalibrate = false;
    bool _autoClearAlarm = false;

public:
    // Define whether automatic calibration is enabled. Forced calibration mode is not an enumerated option in flux.
    // Default value is Disabled
    flxPropertyRWBool<flxDevPASCO2V01, &flxDevPASCO2V01::get_auto_calibrate, &flxDevPASCO2V01::set_auto_calibrate> autoCalibrate;

    // Define the sensor's pressure reference compensation in hPA. Valid range is 750 hPa to 1150 hPa.
    // Default value is 1015 hPa
    flxPropertyRWUint<flxDevPASCO2V01, &flxDevPASCO2V01::get_pressure_reference, &flxDevPASCO2V01::set_pressure_reference> pressureReference = {750, 1150};

    // Define whether an alarm should be raised if it goes above a threshold defined below.
    // Default valued is Disabled
    flxPropertyRWBool<flxDevPASCO2V01, &flxDevPASCO2V01::get_alarm_enabled, &flxDevPASCO2V01::set_alarm_enabled> alarmEnabled;

    // Define whether automatic alarm clearing happens when the ppm value drops below the alarm threshold
    // Default is Disabled
    flxPropertyRWBool<flxDevPASCO2V01, &flxDevPASCO2V01::get_alarm_auto_clear, &flxDevPASCO2V01::set_alarm_auto_clear> autoClearAlarm;

    // Define the CO2 alarm level reference. When the CO2 level goes above this, the alarm bit is set.
    // Default threshold value is 0
    flxPropertyRWUint<flxDevPASCO2V01, &flxDevPASCO2V01::get_alarm_threshold, &flxDevPASCO2V01::set_alarm_threshold> alarmThreshold = {0, 30000};

    // Define the measurement period in seconds. Valid range is 5s - 4095s.
    // Default value is 10s
    flxPropertyRWUint<flxDevPASCO2V01, &flxDevPASCO2V01::get_measurement_period, &flxDevPASCO2V01::set_measurement_period> measurementPeriod = {XENSIV_PASCO2_MEAS_RATE_MIN, XENSIV_PASCO2_MEAS_RATE_MAX};

    // Define output parameters and specify the function called to get that parameter.
    flxParameterOutUint<flxDevPASCO2V01, &flxDevPASCO2V01::read_CO2> co2PPM;
    flxParameterOutBool<flxDevPASCO2V01, &flxDevPASCO2V01::read_alarm_status> alarmStatus;
};