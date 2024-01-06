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
 * QwiicDevACS37800.cpp
 *
 *  Device object for the ACS37800 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevACS37800.h"

#define kACS37800AddressDefault 0x60

// Define our class static variables - allocs storage for them. Note, adding support for 0x60 - 0x63

uint8_t flxDevACS37800::defaultDeviceAddress[] = {kACS37800AddressDefault, kACS37800AddressDefault + 1,
                                                  kACS37800AddressDefault + 2, kACS37800AddressDefault + 3,
                                                  kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevACS37800);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevACS37800::flxDevACS37800()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ACS37800 Power Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    flxRegister(volts, "Voltage", "Volts (Instantaneous)");
    flxRegister(amps, "Current", "Amps (Instantaneous)");
    flxRegister(watts, "Power", "Watts (Instantaneous)");
    flxRegister(voltsRMS, "Voltage (RMS)", "Volts (Root Mean Square)");
    flxRegister(ampsRMS, "Current (RMS)", "Amps (Root Mean Square)");
    flxRegister(powerActive, "Power (Active)", "Watts");
    flxRegister(powerReactive, "Power (Reactive)", "VAR");
    flxRegister(powerApparent, "Power (Apparent)", "VA");
    flxRegister(powerFactor, "Power Factor", "Power Factor");
    flxRegister(positiveAngle, "Power Angle", "True: Lagging; False: Leading");
    flxRegister(positivePowerFactor, "Power Factor Sign", "True: Consumed; False: Generated");

    // Register properties
    flxRegister(numberOfSamples, "Number of samples",
                "The number of samples used in RMS calculations. For DC measurement: set to 1023");
    flxRegister(bypassNenable, "Bypass n enable",
                "Defines how the RMS is calculated. For DC measurement: set to N Samples");
    flxRegister(senseResistance, "Sense resistance", "Define the voltage sense resistance (Ohms)");
    flxRegister(dividerResistance, "Divider resistance", "Define the voltage divider resistance (Ohms)");
    flxRegister(currentRange, "Current range", "Define the sensor current range (Amps)");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevACS37800::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // NOTE: This device could clash with the MCP9600 or the VEML4040. Get a definitive ID, the
    //       confidence value of this device is FUZZY

    // ACS37800
    // Read EEPROM address 0x0B and check the two ECC bits are clear. Not a great check, but something...
    uint8_t reg0B[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    bool couldBe37800 = i2cDriver.readRegisterRegion(address, 0x0B, reg0B, 4); // 32-bits, little endian
    couldBe37800 &= (reg0B[3] & 0x0A) == 0;                                    // Check the two ECC bits are clear
    return couldBe37800;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevACS37800::onInitialize(TwoWire &wirePort)
{
    return ACS37800::begin(address(), wirePort);
}

// GETTER methods for output params
float flxDevACS37800::read_volts()
{
    if (!_volts)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _amps = true;
            _watts = true;
        }
    }
    _volts = false;
    return _theVolts;
}
float flxDevACS37800::read_amps()
{
    if (!_amps)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _volts = true;
            _watts = true;
        }
    }
    _amps = false;
    return _theAmps;
}
float flxDevACS37800::read_watts()
{
    if (!_watts)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _volts = true;
            _amps = true;
        }
    }
    _watts = false;
    return _theWatts;
}
float flxDevACS37800::read_volts_rms()
{
    if (!_volts_rms)
    {
        if (ACS37800::readRMS(&_theVoltsRMS, &_theAmpsRMS) == 0)
        {
            _amps_rms = true;
        }
    }
    _volts_rms = false;
    return _theVoltsRMS;
}
float flxDevACS37800::read_amps_rms()
{
    if (!_amps_rms)
    {
        if (ACS37800::readRMS(&_theVoltsRMS, &_theAmpsRMS) == 0)
        {
            _volts_rms = true;
        }
    }
    _amps_rms = false;
    return _theAmpsRMS;
}
float flxDevACS37800::read_power_active()
{
    if (!_active)
    {
        if (ACS37800::readPowerActiveReactive(&_theActive, &_theReactive) == 0)
        {
            _reactive = true;
        }
    }
    _active = false;
    return _theActive;
}
float flxDevACS37800::read_power_reactive()
{
    if (!_reactive)
    {
        if (ACS37800::readPowerActiveReactive(&_theActive, &_theReactive) == 0)
        {
            _active = true;
        }
    }
    _reactive = false;
    return _theReactive;
}
float flxDevACS37800::read_power_apparent()
{
    if (!_apparent)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _factor = true;
            _posAngle = true;
            _posPF = true;
        }
    }
    _apparent = false;
    return _theApparent;
}
float flxDevACS37800::read_power_factor()
{
    if (!_factor)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _posAngle = true;
            _posPF = true;
        }
    }
    _factor = false;
    return _theFactor;
}
bool flxDevACS37800::read_pos_angle()
{
    if (!_posAngle)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _factor = true;
            _posPF = true;
        }
    }
    _posAngle = false;
    return _thePosAngle;
}
bool flxDevACS37800::read_pos_power_factor()
{
    if (!_posPF)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _factor = true;
            _posAngle = true;
        }
    }
    _posPF = false;
    return _thePosPF;
}

// methods used to get values for our RW properties

uint flxDevACS37800::get_number_of_samples()
{
    if (isInitialized())
        ACS37800::getNumberOfSamples(&_n);
    return _n;
}
void flxDevACS37800::set_number_of_samples(uint numSamples)
{
    _n = numSamples;
    if (isInitialized())
        ACS37800::setNumberOfSamples(numSamples, true); // Set in EEPROM too
}
uint8_t flxDevACS37800::get_bypass_n_enable()
{
    if (isInitialized())
        ACS37800::getBypassNenable(&_bypassNenable);
    return ((uint8_t)_bypassNenable);
}
void flxDevACS37800::set_bypass_n_enable(uint8_t enable)
{
    _bypassNenable = (bool)enable;
    if (isInitialized())
        ACS37800::setBypassNenable(_bypassNenable, true); // Set in EEPROM too
}
float flxDevACS37800::get_sense_resistance()
{
    return _senseResistance;
}
void flxDevACS37800::set_sense_resistance(float res)
{
    _senseResistance = res;
    if (isInitialized())
        ACS37800::setSenseRes(res);
}
float flxDevACS37800::get_divider_resistance()
{
    return _dividerResistance;
}
void flxDevACS37800::set_divider_resistance(float res)
{
    _dividerResistance = res;
    if (isInitialized())
        ACS37800::setDividerRes(res);
}
float flxDevACS37800::get_current_range()
{
    return _currentRange;
}
void flxDevACS37800::set_current_range(float range)
{
    _currentRange = range;
    if (isInitialized())
        ACS37800::setCurrentRange(range);
}
