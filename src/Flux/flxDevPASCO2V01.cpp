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
flxDevPASCO2V01::flxDevPASCO2V01() : theSensor(nullptr)
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("XENSIV PAS CO2 Sensor");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties

    // Register Properties
    flxRegister(autoCalibrate, "Automatic Calibration", "Enable / disable automatic calibration");
    flxRegister(calibrationReference, "Calibration Reference", "Define the calibration ppm. You shouldn't need to change this.");
    flxRegister(pressureReference, "Pressure Reference", "Define the reference pressure in hPa");
    flxRegister(measurementPeriod, "Measurement Period", "Define the measurement period in seconds");

    // Register Parameters
    flxRegister(co2PPM, "CO2 (PPM)", "The CO2 concentration in Parts Per Million");
}

flxDevPASCO2V01::~flxDevPASCO2V01()
{
    delete theSensor;
}

//-----------------------------------------------------------------------------
// Static method used to determine if device is connected before creating this object (if creating dynamically)
bool flxDevPASCO2V01::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;
    uint8_t defaultProdID = 0x4F;
    uint8_t readProdID;

    if(!i2cDriver.readRegister(address, XENSIV_PASCO2_REG_PROD_ID, &readProdID)) {
        flxLog_E("PASCO2V01::isConnected: Failed to read prodID.");
        return false;
    }
    
    return (readProdID == defaultProdID);
}

//-----------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialize the underlying device library/driver
bool flxDevPASCO2V01::onInitialize(TwoWire &wirePort)
{
    int32_t ret = XENSIV_PASCO2_OK;
    uint8_t retries = 3;

    // Create instance of arduino object
    theSensor = new PASCO2Ino(&wirePort);

    do
    {
        // Begin function sets the device into idle mode.
        ret = theSensor->begin();
        if (XENSIV_PASCO2_OK != ret) {
            retries--;
            flxLog_W("PASCO2V01::onInitialize: Begin #%d failed, retrying...", (4-retries));
            delay(200);
        }
    } while ((XENSIV_PASCO2_OK != ret) && (retries > 0));

    if (XENSIV_PASCO2_OK != ret) {
        flxLog_E("PASCO2V01::onInitialize: Sensor failed to respond!");
        return false;
    }

    set_auto_calibrate(_autoCalibrate);
    set_pressure_reference(_pressureReference);

    // Lets set it to the default for our startup profile.
    ret = theSensor->startMeasure(_measurementPeriod);

    if(XENSIV_PASCO2_OK != ret) {
        flxLog_E("PASCO2V01::onInitialize: Sensor failed to begin measuring.");
        return false;
    }

    return true;
}

// GETTER methods for output params
uint flxDevPASCO2V01::read_CO2()
{
    static int16_t co2InPPM = 0;
    static uint32_t prevMillis = 0;

    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;
        
        uint32_t currentMillis = millis();

        if (currentMillis - prevMillis >= (_measurementPeriod*1000)) {
            prevMillis = currentMillis;
            ret = theSensor->getCO2(co2InPPM);
        }
        

        if (XENSIV_PASCO2_OK != ret) {
            flxLog_E("PASCO2V01::read_CO2: Failed. Returned error code %d. Logging last received value.", ret);
        }
    }

    return ((uint) co2InPPM);
}

//-----------------------------------------------------------------------------
// RW Properties

bool flxDevPASCO2V01::get_auto_calibrate()
{
    return _autoCalibrate;
}

uint flxDevPASCO2V01::get_calibration_reference()
{
    return _calibrationReference;
}

uint flxDevPASCO2V01::get_pressure_reference()
{
    return _pressureReference;
}

uint flxDevPASCO2V01::get_measurement_period()
{
    return _measurementPeriod;
}

void flxDevPASCO2V01::set_auto_calibrate(bool enabled)
{
    static bool constructed = false;
    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;

        bool prevCalibration = _autoCalibrate;
        _autoCalibrate = enabled;

        ABOC_t aboc = enabled ? XENSIV_PASCO2_BOC_CFG_AUTOMATIC : XENSIV_PASCO2_BOC_CFG_DISABLE;

        ret = theSensor->setABOC(aboc, _calibrationReference);

        if(XENSIV_PASCO2_OK != ret) {
            _autoCalibrate = prevCalibration;
            flxLog_W("PASCO2V01::set_auto_calibrate: Could not set calibration.");
        }
        
    }
    else {
        if(!constructed) {
            constructed = true;
        }
        else {
            flxLog_E("PASCO2V01::set_auto_calibrate: Failed! Sensor is nullptr.");
        }
    }
}

void flxDevPASCO2V01::set_calibration_reference(uint reference)
{
    static bool constructed = false;
    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;

        uint16_t prevCalReference = _calibrationReference;
        _calibrationReference = ((uint16_t) reference);

        ABOC_t aboc = _autoCalibrate ? XENSIV_PASCO2_BOC_CFG_AUTOMATIC : XENSIV_PASCO2_BOC_CFG_DISABLE;

        ret = theSensor->setABOC(aboc, _calibrationReference);

        if(XENSIV_PASCO2_OK != ret) {
            _calibrationReference = prevCalReference;
            flxLog_W("PASCO2V01::set_calibration_reference: Could not set calibration reference value");
        }
    }
    else {
        if(!constructed) {
            constructed = true;
        }
        else {
            flxLog_E("PASCO2V01::set_calibration_reference: Failed! Sensor is nullptr.");
        }
    }
}

void flxDevPASCO2V01::set_pressure_reference(uint reference)
{
    static bool constructed = false;
    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;

        uint16_t prevPressure = _pressureReference;
        _pressureReference = ((uint16_t) reference);

        ret = theSensor->setPressRef(_pressureReference);

        if(XENSIV_PASCO2_OK != ret) {
            _pressureReference = prevPressure;
            flxLog_W("PASCO2V01::set_pressure_reference: Could not set pressure reference.");
        }
    }
    else {
        if(!constructed) {
            constructed = true;
        }
        else {
            flxLog_E("PASCO2V01::set_pressure_reference: Failed! Sensor is nullptr.");
        }
    }
}

void flxDevPASCO2V01::set_measurement_period(uint period)
{
    static bool constructed = false;
    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;
        uint prevPeriod = _measurementPeriod;

        ret = theSensor->stopMeasure();

        _measurementPeriod = period;

        ret = theSensor->startMeasure(_measurementPeriod);

        if(XENSIV_PASCO2_OK != ret){
            _measurementPeriod = prevPeriod;
            flxLog_W("PASCO2V01::set_measurement_period: Could not set measurement period.");
        }
    }
    else {
        if(!constructed) {
            constructed = true;
        }
        else {
            flxLog_E("PASCO2V01::set_measurement_period: Failed! Sensor is nullptr.");
        }
    }
}
