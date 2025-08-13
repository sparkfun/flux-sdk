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
 *  flxDevBMV080.h
 *
 *  Spark Device object for the BMV080 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_BMV080_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kBMV080DeviceName "BMV080"

// Minimum duty cycle in seconds to take into account smallest integration time (secs)
const uint16_t kBMV080DutyCycleMin = 3;
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBMV080 : public flxDeviceI2CType<flxDevBMV080>, public SparkFunBMV080
{

  public:
    flxDevBMV080();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidencePing;
    }

    static const char *getDeviceName()
    {
        return kBMV080DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // called before reading values
    bool execute(void);

    void restoreComplete(void);

  private:
    void startSensor(void);
    void stopSensor(void);

    // Define our output parameters - these are the values we will return

    float read_pm10(void)
    {
        return SparkFunBMV080::PM10();
    }
    float read_pm25(void)
    {
        return SparkFunBMV080::PM25();
    }
    float read_pm1(void)
    {
        return SparkFunBMV080::PM1();
    }
    bool is_obstructed(void)
    {
        return SparkFunBMV080::isObstructed();
    }

    // Property Getters and Setters
    bool get_enable_obstructed(void);
    // internal method
    void set_enable_obstructed(bool);
    bool _obstructedEnabled; // Flag to enable/disable obstructed detection

    // Operating Mode - continuous or duty cycle
    uint8_t get_operating_mode(void);
    void set_operating_mode(uint8_t mode);
    uint8_t _operatingMode;

    // duty cycle
    uint16_t get_duty_cycle(void);
    void set_duty_cycle(uint16_t dutyCycle);
    uint16_t _dutyCycle; // in seconds, 0 means continuous mode

    // vibration filtering
    void set_enable_vibration_filter(bool enable);
    bool get_enable_vibration_filter(void);
    bool _vibrationFilterEnabled;

    // Integration Time (s)
    void set_integration_time(uint16_t integrationTime);
    uint16_t get_integration_time(void);
    uint16_t _integrationTime; // in seconds

    bool _isRunning; // Flag to indicate if the device is running
    uint8_t _updateCnt;

    void beginUpdate(void);
    void endUpdate(void);

  public:
    // Properties
    flxPropertyRWBool<flxDevBMV080, &flxDevBMV080::get_enable_obstructed, &flxDevBMV080::set_enable_obstructed>
        enableObstructed;

    flxPropertyRWUInt8<flxDevBMV080, &flxDevBMV080::get_operating_mode, &flxDevBMV080::set_operating_mode>
        operatingMode = {SF_BMV080_MODE_CONTINUOUS,
                         {{"Continuous", SF_BMV080_MODE_CONTINUOUS}, {"Duty Cycle", SF_BMV080_MODE_DUTY_CYCLE}}};

    flxPropertyRWUInt16<flxDevBMV080, &flxDevBMV080::get_duty_cycle, &flxDevBMV080::set_duty_cycle> dutyCycle = {
        kBMV080DutyCycleMin, 3600}; // 1 to 3600 seconds}

    flxPropertyRWBool<flxDevBMV080, &flxDevBMV080::get_enable_vibration_filter,
                      &flxDevBMV080::set_enable_vibration_filter>
        enableVibrationFilter;

    flxPropertyRWUInt16<flxDevBMV080, &flxDevBMV080::get_integration_time, &flxDevBMV080::set_integration_time>
        integrationTime = {1, 120}; // 1 to 120 seconds

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm10> PM10;
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm25> PM25;
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm1> PM1;
    flxParameterOutBool<flxDevBMV080, &flxDevBMV080::is_obstructed> obstructed;
};
