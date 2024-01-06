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
 *  flxDevVL53L1X.h
 *
 *  Spark Device object for the VL53L1X device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevVL53L1X.h"

// Define our class static variables - allocs storage for them

#define kVL53L1XAddressDefault 0x29 // Unshifted

uint8_t flxDevVL53L1X::defaultDeviceAddress[] = {kVL53L1XAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevVL53L1X);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevVL53L1X::flxDevVL53L1X()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VL53L1X Distance Sensor");

    // Register parameters
    flxRegister(distance, "Distance (mm)", "The measured distance in mm");
    flxRegister(rangeStatus, "Range Status", "The measurement range status : 0 = good");
    flxRegister(signalRate, "Signal Rate", "The measurement signal rate");

    // Register read-write properties
    flxRegister(distanceMode, "Distance Mode", "The selected distance mode: short or long");
    flxRegister(intermeasurementPeriod, "Inter-Measurement Period (ms)",
                "The inter-measurement period in milliseconds");
    flxRegister(crosstalk, "Crosstalk", "The crosstalk compensation in counts-per-second");
    flxRegister(offset, "Offset", "The measurement offset in mm");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevVL53L1X::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t modelId[2] = {(uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID >> 8),
                          (uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID & 0xFF)};
    bool couldBeVL53L = i2cDriver.write(address, modelId, 2);
    uint8_t idReg[3];
    couldBeVL53L &= i2cDriver.receiveResponse(address, idReg, 3) == 3;
    uint16_t idReg16 = (((uint16_t)idReg[0]) << 8) | idReg[1];
    couldBeVL53L &= ((idReg16 == 0xEACC) || (idReg16 == 0xEBAA));

    return (couldBeVL53L);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevVL53L1X::onInitialize(TwoWire &wirePort)
{

    bool status = (SFEVL53L1X::begin(wirePort) == 0);
    if (status)
    {
        (_shortDistanceMode ? SFEVL53L1X::setDistanceModeShort() : SFEVL53L1X::setDistanceModeLong());
        // Intermeasurement Period limit for short distance mode is 20:1000. For long distance mode, it is 140:1000.
        (_shortDistanceMode ? intermeasurementPeriod.setDataLimitRange(20, 1000)
                            : intermeasurementPeriod.setDataLimitRange(140, 1000));
        uint16_t imp = SFEVL53L1X::getIntermeasurementPeriod();
        if (!_shortDistanceMode)
            if (imp < 140)
                SFEVL53L1X::setIntermeasurementPeriod(140);
        SFEVL53L1X::startRanging();
    }
    return status;
}

// GETTER methods for output params
uint flxDevVL53L1X::read_distance()
{
    return SFEVL53L1X::getDistance();
}

uint flxDevVL53L1X::read_range_status()
{
    return SFEVL53L1X::getRangeStatus();
}

uint flxDevVL53L1X::read_signal_rate()
{
    return SFEVL53L1X::getSignalRate();
}

// methods for read-write properties
uint8_t flxDevVL53L1X::get_distance_mode()
{
    if (_shortDistanceMode)
        return DISTANCE_SHORT;
    return DISTANCE_LONG;
}

void flxDevVL53L1X::set_distance_mode(uint8_t mode)
{
    _shortDistanceMode = (mode == DISTANCE_SHORT);
    if (isInitialized())
    {
        SFEVL53L1X::stopRanging();
        (_shortDistanceMode ? SFEVL53L1X::setDistanceModeShort() : SFEVL53L1X::setDistanceModeLong());
        // Intermeasurement Period limit for short distance mode is 20:1000. For long distance mode, it is 140:1000.
        (_shortDistanceMode ? intermeasurementPeriod.setDataLimitRange(20, 1000)
                            : intermeasurementPeriod.setDataLimitRange(140, 1000));
        uint16_t imp = SFEVL53L1X::getIntermeasurementPeriod();
        if (!_shortDistanceMode)
            if (imp < 140)
                SFEVL53L1X::setIntermeasurementPeriod(140);
        SFEVL53L1X::startRanging();
    }
}

uint16_t flxDevVL53L1X::get_intermeasurment_period()
{
    if (isInitialized())
        _intermeasurementPeriod = SFEVL53L1X::getIntermeasurementPeriod();
    return _intermeasurementPeriod;
}

void flxDevVL53L1X::set_intermeasurment_period(uint16_t period)
{
    // Validate period. This is probably redundant - given the data limit range?
    if (period < 20)
        period = 20;
    if (!_shortDistanceMode)
        if (period < 140)
            period = 140;
    if (period > 1000)
        period = 1000;
    _intermeasurementPeriod = period;

    if (isInitialized())
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setIntermeasurementPeriod(period);
        SFEVL53L1X::startRanging();
    }
}

uint16_t flxDevVL53L1X::get_crosstalk()
{
    if (isInitialized())
        _crosstalk = SFEVL53L1X::getXTalk();
    return _crosstalk;
}

void flxDevVL53L1X::set_crosstalk(uint16_t level)
{
    _crosstalk = level;
    if (isInitialized())
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setXTalk(level);
        SFEVL53L1X::startRanging();
    }
}

uint16_t flxDevVL53L1X::get_offset()
{
    if (isInitialized())
        _offset = SFEVL53L1X::getOffset();
    return _offset;
}

void flxDevVL53L1X::set_offset(uint16_t offset)
{
    _offset = offset;
    if (isInitialized())
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setOffset(offset);
        SFEVL53L1X::startRanging();
    }
}
