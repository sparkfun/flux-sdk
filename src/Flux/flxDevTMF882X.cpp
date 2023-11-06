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
 *  flxDevTMF882X.h
 *
 *  Spark Device object for the TMF882X device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevTMF882X.h"

// Define our class static variables - allocs storage for them

#define kTMF882XAddressDefault 0x41 // Unshifted

#define kTMF882XIDReg 0xE3 // Reads 0x08. Ignore bits 6 and 7.

uint8_t flxDevTMF882X::defaultDeviceAddress[] = {kTMF882XAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevTMF882X);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevTMF882X::flxDevTMF882X()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("TMF882x Time-Of-Flight Sensor");

    // Register parameters
    flxRegister(confidence, "Confidence", "The distance measurement confidence");
    flxRegister(distance, "Distance (mm)", "The measured distance in mm");
    flxRegister(channel, "Channel", "The measurement channel");
    flxRegister(subCapture, "Sub Capture", "The measurement sub-capture");
    flxRegister(photonCount, "Photon Count", "The measurement photon count");
    flxRegister(refPhotonCount, "Ref Photon Count", "The reference photon count");
    flxRegister(ambientLight, "Ambient Light", "The ambient light level");

    flxRegister(reportPeriod, "Report Period (ms)", "The reporting period in milliseconds)");

    flxRegister(factoryCalibration, "Perform Factory Calibration", "Perform Factory Calibration - requires minimal ambient light and no target within 40 cm");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevTMF882X::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t idReg;
    bool couldBeTMF882 = i2cDriver.readRegister(address, kTMF882XIDReg, &idReg);
    couldBeTMF882 &= (idReg & 0x3F) == 0x08;

    return (couldBeTMF882);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevTMF882X::onInitialize(TwoWire &wirePort)
{

    return SparkFun_TMF882X::begin(wirePort, address());

}

//methods for our read-write properties
uint16_t flxDevTMF882X::get_report_period()
{
    if (isInitialized())
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X set_report_period - unable to get device configuration");
            return _reportPeriod;
        }
        
        _reportPeriod = tofConfig.report_period_ms;
    }
    return _reportPeriod;
}
void flxDevTMF882X::set_report_period(uint16_t period)
{
    if (isInitialized())
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X set_report_period - unable to get device configuration");
            return;
        }
        
        // Change the APP configuration
        //  - set the reporting period
        tofConfig.report_period_ms = period;
        _reportPeriod = period;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X set_report_period- unable to set device configuration");
            return;
        }
    }
}

//methods for write properties
void flxDevTMF882X::factory_calibration()
{
    if (isInitialized())
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X factory_calibration - unable to get device configuration");
            return;
        }
        
        // Change the APP configuration
        //  - set the reporting period to 460 milliseconds
        //  - set the iterations to 4,000,000 (4M) to perform factory calibration
        tofConfig.report_period_ms = 460;
        uint16_t originalIterations = tofConfig.kilo_iterations;
        tofConfig.kilo_iterations = 4000;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X factory_calibration - unable to set device configuration");
            return;
        }

        struct tmf882x_mode_app_calib factoryCal;
        if (!SparkFun_TMF882X::factoryCalibration(factoryCal))
            flxLog_E("TMF882X factory_calibration - factory calibration failed");

        tofConfig.report_period_ms = _reportPeriod;
        tofConfig.kilo_iterations = originalIterations;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            flxLog_E("TMF882X factory_calibration - unable to restore device configuration");
            return;
        }
    }
}

// GETTER methods for output params
bool flxDevTMF882X::read_confidence(flxDataArrayUint *conf)
{
    static uint32_t theConfidence[TMF882X_MAX_MEAS_RESULTS] = {0};

    if (!_confidence)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _confidence = false;

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theConfidence[result] = _results.results[result].confidence;
    }

    conf->set(theConfidence, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool flxDevTMF882X::read_distance(flxDataArrayUint *dist)
{
    static uint32_t theDistance[TMF882X_MAX_MEAS_RESULTS] = {0};

    if (!_distance)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _distance = false;

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theDistance[result] = _results.results[result].distance_mm;
    }

    dist->set(theDistance, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool flxDevTMF882X::read_channel(flxDataArrayUint *chan)
{
    static uint32_t theChannel[TMF882X_MAX_MEAS_RESULTS] = {0};

    if (!_channel)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _channel = false;

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theChannel[result] = _results.results[result].channel;
    }

    chan->set(theChannel, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool flxDevTMF882X::read_sub_capture(flxDataArrayUint *sub)
{
    static uint32_t theSubCapture[TMF882X_MAX_MEAS_RESULTS] = {0};

    if (!_sub_capture)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _sub_capture = false;

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theSubCapture[result] = _results.results[result].sub_capture;
    }

    sub->set(theSubCapture, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
uint flxDevTMF882X::read_photon_count()
{
    if (!_photon_count)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _photon_count = false;
    return _results.photon_count;
}
uint flxDevTMF882X::read_ref_photon_count()
{
    if (!_ref_photon_count)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ambient_light = true;
        }
    }
    _ref_photon_count = false;
    return _results.ref_photon_count;
}
uint flxDevTMF882X::read_ambient_light()
{
    if (!_ambient_light)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
        }
    }
    _ambient_light = false;
    return _results.ambient_light;
}

