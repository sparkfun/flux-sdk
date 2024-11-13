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
 *  flxDevPASCO2V01.cpp
 *
 *  Spark Device object for the Infineon XENSIV PAS CO2 device.
 *
 *
 *
 */
#include <Arduino.h>

#include "flxDevPASCO2V01.h"

#define kPASCO2V01AddressDefault XENSIV_PASCO2_I2C_ADDR // 0x28U

#define kDefaultProdID 0x4F

// Define our class static variables - allocs storage for them

uint8_t flxDevPASCO2V01::defaultDeviceAddress[] = {kPASCO2V01AddressDefault, kSparkDeviceAddressNull};

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevPASCO2V01);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevPASCO2V01::flxDevPASCO2V01() : _theSensor(nullptr)
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "XENSIV PAS CO2 Sensor");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties

    // Register Properties
    flxRegister(autoCalibrate, "Automatic Calibration", "Enable / disable automatic calibration");
    flxRegister(calibrationReference, "Calibration Reference",
                "Define the calibration ppm. You shouldn't need to change this.");
    flxRegister(pressureReference, "Pressure Reference", "Define the reference pressure in hPa");
    flxRegister(measurementPeriod, "Measurement Period", "Define the measurement period in seconds");

    // Register Parameters
    flxRegister(co2PPM, "CO2 (PPM)", "The CO2 concentration in Parts Per Million");
}

flxDevPASCO2V01::~flxDevPASCO2V01()
{
    if (_theSensor != nullptr)
        delete _theSensor;
}

//-----------------------------------------------------------------------------
// Static method used to determine if device is connected before creating this object (if creating dynamically)
bool flxDevPASCO2V01::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t readProdID;

    if (!i2cDriver.readRegister(address, XENSIV_PASCO2_REG_PROD_ID, &readProdID))
    {
        flxLog_E("PASCO2V01::isConnected: Failed to read prodID.");
        return false;
    }

    return (readProdID == kDefaultProdID);
}

//-----------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialize the underlying device library/driver
bool flxDevPASCO2V01::onInitialize(TwoWire &wirePort)
{
    uint8_t retries = 3;

    // Create instance of arduino object
    _theSensor = new PASCO2Ino(&wirePort);

    if (_theSensor == nullptr)
        return false;

    while (XENSIV_PASCO2_OK != _theSensor->begin())
    {
        delay(200);
        retries--;
        flxLog_D("PASCO2V01::onInitialize: Begin #%d failed, retrying...", (3 - retries));
        if (retries <= 0)
        {
            flxLog_E("PASCO2V01::onInitialize: Sensor failed to respond!");
            return false;
        }
    }

    _sensorIsInitialized = true;

    set_auto_calibrate(_autoCalibrate);
    set_pressure_reference(_pressureReference);

    // Lets set it to the default for our startup profile.
    if (XENSIV_PASCO2_OK != _theSensor->startMeasure(_measurementPeriod))
    {
        flxLog_E("PASCO2V01::onInitialize: Sensor failed to begin measuring.");
        return false;
    }

    _sensorIsMeasuring = true;

    return true;
}

// GETTER methods for output params
uint32_t flxDevPASCO2V01::read_CO2()
{
    if (_theSensor == nullptr)
    {
        flxLog_E("PASCO2V01::read_CO2: Failed! Sensor is nullptr.");
        return ((uint)_co2InPPM);
    }

    if (!_sensorIsMeasuring)
    {
        flxLog_W("PASCO2V01::read_CO2: Sensor is not measuring, attempting to start.");
        if (XENSIV_PASCO2_OK != _theSensor->startMeasure(_measurementPeriod))
        {
            flxLog_E("PASCO2::read_CO2: Sensor failed to restart. Logging last received value.");
            return ((uint)_co2InPPM);
        }
        _sensorIsMeasuring = true;
    }

    uint32_t currentMillis = millis();

    if (currentMillis - _millisSinceLastMeasure >= (_measurementPeriod * 1000))
    {
        _millisSinceLastMeasure = currentMillis;
        if (XENSIV_PASCO2_OK != _theSensor->getCO2(_co2InPPM))
        {
            flxLog_E("PASCO2V01::read_CO2: Failed to read sensor. Logging last received value.");
        }
    }

    return ((uint)_co2InPPM);
}

//-----------------------------------------------------------------------------
// RW Properties

bool flxDevPASCO2V01::get_auto_calibrate()
{
    return _autoCalibrate;
}

uint32_t flxDevPASCO2V01::get_calibration_reference()
{
    return _calibrationReference;
}

uint32_t flxDevPASCO2V01::get_pressure_reference()
{
    return _pressureReference;
}

uint32_t flxDevPASCO2V01::get_measurement_period()
{
    return _measurementPeriod;
}

void flxDevPASCO2V01::set_auto_calibrate(bool enabled)
{
    if (_theSensor == nullptr)
    {
        if (_sensorIsInitialized)
            flxLog_E("PASCO2::set_auto_calibrate: Failed! Sensor is nullptr.");
        return;
    }

    bool prevCalibration = _autoCalibrate;
    _autoCalibrate = enabled;

    if (XENSIV_PASCO2_OK !=
        _theSensor->setABOC((enabled ? XENSIV_PASCO2_BOC_CFG_AUTOMATIC : XENSIV_PASCO2_BOC_CFG_DISABLE),
                            _calibrationReference))
    {
        _autoCalibrate = prevCalibration;
        flxLog_W("PASCO2V01::set_auto_calibrate: Could not set calibration.");
    }
}

void flxDevPASCO2V01::set_calibration_reference(uint32_t reference)
{
    if (_theSensor == nullptr)
    {
        if (_sensorIsInitialized)
            flxLog_E("PASCO2V01::set_calibration_reference: Failed! Sensor is nullptr.");
        return;
    }

    uint16_t prevCalReference = _calibrationReference;
    _calibrationReference = ((uint16_t)reference);

    if (XENSIV_PASCO2_OK !=
        _theSensor->setABOC((_autoCalibrate ? XENSIV_PASCO2_BOC_CFG_AUTOMATIC : XENSIV_PASCO2_BOC_CFG_DISABLE),
                            _calibrationReference))
    {
        _calibrationReference = prevCalReference;
        flxLog_W("PASCO2V01::set_calibration_reference: Could not set calibration reference value");
    }
}

void flxDevPASCO2V01::set_pressure_reference(uint32_t reference)
{
    if (_theSensor == nullptr)
    {
        if (_sensorIsInitialized)
            flxLog_E("PASCO2V01::set_pressure_reference: Failed! Sensor is nullptr.");
        return;
    }

    uint16_t prevPressure = _pressureReference;
    _pressureReference = ((uint16_t)reference);

    if (XENSIV_PASCO2_OK != _theSensor->setPressRef(_pressureReference))
    {
        _pressureReference = prevPressure;
        flxLog_W("PASCO2V01::set_pressure_reference: Could not set pressure reference.");
    }
}

void flxDevPASCO2V01::set_measurement_period(uint32_t period)
{
    if (_theSensor == nullptr)
    {
        if (_sensorIsInitialized)
            flxLog_E("PASCO2V01::set_measurement_period: Failed! Sensor is nullptr.");
        return;
    }

    uint prevPeriod = _measurementPeriod;
    _measurementPeriod = period;

    if (_sensorIsMeasuring)
    {
        if (XENSIV_PASCO2_OK != _theSensor->stopMeasure())
        {
            flxLog_E("PASCO2V01::set_measurement_period: Could not stop measurements.");
            return;
        }
        _sensorIsMeasuring = false;
    }

    if (XENSIV_PASCO2_OK != _theSensor->startMeasure(_measurementPeriod))
    {
        _measurementPeriod = prevPeriod;
        flxLog_E("PASCO2V01::set_measurement_period: Could not set measurement period and start measuring.");
    }

    _sensorIsMeasuring = true;
}
