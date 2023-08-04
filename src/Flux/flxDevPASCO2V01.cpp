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

#define kPASCO2V01AddressDefault XENSIV_PASCO2_I2C_ADDR

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
    flxRegister(pressureReference, "Pressure Reference", "Define the reference pressure in hPa");
    flxRegister(alarmEnabled, "Alarm Enabled", "Enable / disable the alarm feature");
    flxRegister(alarmThreshold, "Alarm Threshold", "Define the alarm threshold value in ppm");
    flxRegister(measurementPeriod, "Measurement Period", "Define the measurement period in seconds");

    // Register Parameters
    flxRegister(co2PPM, "CO2 (PPM)", "The CO2 concentration in Parts Per Million");
    flxRegister(autoClearAlarm, "Automatic Alarm Clear", "Enable / disable automatically clearing the alarm");
    flxRegister(alarmStatus, "Alarm Status", "Status of alarm indicator");
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

    // TODO: Read Prod ID manually
    

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

    // Create instance of arduino object
    theSensor = new PASCO2Ino(&wirePort);

    // Begin function sets the device into idle mode.
    ret = theSensor->begin();
    
    // TODO: Do the begin a couple times until it responds - base off MAX1704x below
    /* 
    bool success = false;
    uint8_t retries = 3;
    uint16_t version = 0;

    while ((success == false) && (retries > 0))
    {
        if (i2cDriver.readRegister16(address, MAX17043_VERSION, &version, false)) // Attempt to read the version register
        {
            success = true;
        }
        else
        {
            retries--;
            flxLog_W("SFE_MAX1704X::isConnected: retrying...");
            delay(50);
        }
    }

    if (!success) // Return now if the version could not be read
    {
        flxLog_E("SFE_MAX1704X::isConnected: failed to detect IC!");
        return (success);
    }
    */

    // Lets set it to the default for our startup profile.
    ret = theSensor->startMeasure(XENSIV_PASCO2_MEAS_RATE_MIN);

    if(XENSIV_PASCO2_OK != ret)
        return false;

    return true;
}

// GETTER methods for output params
uint flxDevPASCO2V01::read_CO2()
{
    static uint co2InPPM = 0;
    if(theSensor != nullptr) {
        int32_t ret = XENSIV_PASCO2_OK;

        ret = theSensor->getCO2(&co2InPPM);

        if(XENSIV_PASCO2_OK != ret)
        {
            flxLog_E("PASCO2V01::read_CO2: Failed. Returned error %d. Logging last known good value.", ret);
        }
    }
    return co2InPPM;
}

bool flxDevPASCO2V01::read_alarm_status()
{
    
}

//-----------------------------------------------------------------------------
// RW Properties

bool flxDevPASCO2V01::get_auto_calibrate()
{

}

uint flxDevPASCO2V01::get_pressure_reference()
{

}

bool flxDevPASCO2V01::get_alarm_enabled()
{

}

bool flxDevPASCO2V01::get_alarm_auto_clear()
{

}

uint flxDevPASCO2V01::get_alarm_threshold()
{

}

uint flxDevPASCO2V01::get_measurement_period()
{

}

void flxDevPASCO2V01::set_auto_calibrate(bool)
{

}

void flxDevPASCO2V01::set_pressure_reference(uint)
{

}

void flxDevPASCO2V01::set_alarm_enabled(bool)
{

}

void flxDevPASCO2V01::set_alarm_auto_clear(bool)
{

}

void flxDevPASCO2V01::set_alarm_threshold(uint)
{

}

void flxDevPASCO2V01::set_measurement_period(uint)
{

}
