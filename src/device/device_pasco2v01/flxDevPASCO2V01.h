/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
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
#include <pas-co2-ino.hpp>

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

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

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
    PASCO2Ino *_theSensor;

    // methods used to get values for our output parameters
    uint32_t read_CO2();

    // methods used to get values for our RW properties
    bool get_auto_calibrate();
    uint32_t get_calibration_reference();
    uint32_t get_pressure_reference();
    uint32_t get_measurement_period();

    void set_auto_calibrate(bool);
    void set_calibration_reference(uint32_t);
    void set_pressure_reference(uint32_t);
    void set_measurement_period(uint32_t);

    bool _autoCalibrate = false;

    uint _measurementPeriod = XENSIV_PASCO2_MEAS_RATE_MIN;

    uint16_t _pressureReference = 1015;   // Default value on reset
    uint16_t _calibrationReference = 400; // Default value on reset

    uint32_t _millisSinceLastMeasure = 0;
    int16_t _co2InPPM = 0;

    bool _sensorIsInitialized = false;
    bool _sensorIsMeasuring = false;

  public:
    // Define whether automatic calibration is enabled. Forced calibration mode is not an enumerated option in flux.
    // Default value is Disabled
    flxPropertyRWBool<flxDevPASCO2V01, &flxDevPASCO2V01::get_auto_calibrate, &flxDevPASCO2V01::set_auto_calibrate>
        autoCalibrate;

    // Define the sensor's calibration baseline offset compensation reference. Valid range is 350 ppm to 900 ppm.
    // Default value is 400 ppm
    flxPropertyRWUInt32<flxDevPASCO2V01, &flxDevPASCO2V01::get_calibration_reference,
                        &flxDevPASCO2V01::set_calibration_reference>
        calibrationReference = {_calibrationReference, 350, 900};

    // Define the sensor's pressure reference compensation in hPA. Valid range is 750 hPa to 1150 hPa.
    // Default value is 1015 hPa
    flxPropertyRWUInt32<flxDevPASCO2V01, &flxDevPASCO2V01::get_pressure_reference,
                        &flxDevPASCO2V01::set_pressure_reference>
        pressureReference = {_pressureReference, 750, 1150};

    // Define the measurement period in seconds. Valid range is 5s - 4095s.
    // Default value is 10s
    flxPropertyRWUInt32<flxDevPASCO2V01, &flxDevPASCO2V01::get_measurement_period,
                        &flxDevPASCO2V01::set_measurement_period>
        measurementPeriod = {_measurementPeriod, XENSIV_PASCO2_MEAS_RATE_MIN, XENSIV_PASCO2_MEAS_RATE_MAX};

    // Define output parameters and specify the function called to get that parameter.
    flxParameterOutUInt32<flxDevPASCO2V01, &flxDevPASCO2V01::read_CO2> co2PPM;
};
