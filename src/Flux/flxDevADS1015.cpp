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
 *  flxDevADS1015.cpp
 *
 *  Spark Device object for the ADS1015 A/D converter device.
 *
 *
 */

#include "Arduino.h"

#include "flxDevADS1015.h"

// The ADS1015 supports multiple addresses:
uint8_t flxDevADS1015::defaultDeviceAddress[] = {0x48, 0x49, 0x4A, 0x4B, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevADS1015);

flxDevADS1015::flxDevADS1015()
    : _seType{kADS1015DeviceFloat}, _diffType{kADS1015DeviceFloat}, _sampleRate{ADS1015_CONFIG_RATE_1600HZ},
      _gain{ADS1015_CONFIG_PGA_2}
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("ADS1015 A/D Converter");

    // Register Properties

    flxRegister(sampleRate, "Sample Rate", "The sample rate in Hz");
    flxRegister(gain, "Gain", "Programmable gain setting");

    // Register output params
    flxRegister(channel0_f, "Channel 0 Single-Ended (mV)", "Channel 0 Single-Ended (millivolts)");
    flxRegister(channel1_f, "Channel 1 Single-Ended (mV)", "Channel 1 Single-Ended (millivolts)");
    flxRegister(channel2_f, "Channel 2 Single-Ended (mV)", "Channel 2 Single-Ended (millivolts)");
    flxRegister(channel3_f, "Channel 3 Single-Ended (mV)", "Channel 3 Single-Ended (millivolts)");
    flxRegister(differential_0_minus_1_f, "Differential: P0 N1 (mV)",
                "Differential: Channel 0 - Channel 1 (millivolts)");
    flxRegister(differential_0_minus_3_f, "Differential: P0 N3 (mV)",
                "Differential: Channel 0 - Channel 3 (millivolts)");
    flxRegister(differential_1_minus_3_f, "Differential: P1 N3 (mV)",
                "Differential: Channel 1 - Channel 3 (millivolts)");
    flxRegister(differential_2_minus_3_f, "Differential: P2 N3 (mV)",
                "Differential: Channel 2 - Channel 3 (millivolts)");

    // Register output params - ints!
    flxRegister(channel0_i, "Channel 0 Single-Ended (int)", "Channel 0 Single-Ended (signed)");
    flxRegister(channel1_i, "Channel 1 Single-Ended (int)", "Channel 1 Single-Ended (signed)");
    flxRegister(channel2_i, "Channel 2 Single-Ended (int)", "Channel 2 Single-Ended (signed)");
    flxRegister(channel3_i, "Channel 3 Single-Ended (int)", "Channel 3 Single-Ended (signed)");
    flxRegister(differential_0_minus_1_i, "Differential: P0 N1 (int)", "Differential: Channel 0 - Channel 1 (signed)");
    flxRegister(differential_0_minus_3_i, "Differential: P0 N3 (int)", "Differential: Channel 0 - Channel 3 (signed)");
    flxRegister(differential_1_minus_3_i, "Differential: P1 N3 (int)", "Differential: Channel 1 - Channel 3 (signed)");
    flxRegister(differential_2_minus_3_i, "Differential: P2 N3 (int)", "Differential: Channel 2 - Channel 3 (signed)");

    // Register output params - unsigned ints!
    flxRegister(channel0_u, "Channel 0 Single-Ended (uint)", "Channel 0 Single-Ended (unsigned)");
    flxRegister(channel1_u, "Channel 1 Single-Ended (uint)", "Channel 1 Single-Ended (unsigned)");
    flxRegister(channel2_u, "Channel 2 Single-Ended (uint)", "Channel 2 Single-Ended (unsigned)");
    flxRegister(channel3_u, "Channel 3 Single-Ended (uint)", "Channel 3 Single-Ended (unsigned)");
}
// Function to encapsulate the ops needed to get values from the sensor.
float flxDevADS1015::readf_single_0()
{
    return (ADS1015::getSingleEndedMillivolts(0));
}
float flxDevADS1015::readf_single_1()
{
    return (ADS1015::getSingleEndedMillivolts(1));
}
float flxDevADS1015::readf_single_2()
{
    return (ADS1015::getSingleEndedMillivolts(2));
}
float flxDevADS1015::readf_single_3()
{
    return (ADS1015::getSingleEndedMillivolts(3));
}
float flxDevADS1015::readf_differential_P0_N1()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P0_N1));
}
float flxDevADS1015::readf_differential_P0_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P0_N3));
}
float flxDevADS1015::readf_differential_P1_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P1_N3));
}
float flxDevADS1015::readf_differential_P2_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P2_N3));
}

// Singed Int params
int16_t flxDevADS1015::readi_single_0()
{
    return (ADS1015::getSingleEndedSigned(0));
}
int16_t flxDevADS1015::readi_single_1()
{
    return (ADS1015::getSingleEndedSigned(1));
}
int16_t flxDevADS1015::readi_single_2()
{
    return (ADS1015::getSingleEndedSigned(2));
}
int16_t flxDevADS1015::readi_single_3()
{
    return (ADS1015::getSingleEndedSigned(3));
}
int16_t flxDevADS1015::readi_differential_P0_N1()
{
    return (ADS1015::getDifferential(ADS1015_CONFIG_MUX_DIFF_P0_N1));
}
int16_t flxDevADS1015::readi_differential_P0_N3()
{
    return (ADS1015::getDifferential(ADS1015_CONFIG_MUX_DIFF_P0_N3));
}
int16_t flxDevADS1015::readi_differential_P1_N3()
{
    return (ADS1015::getDifferential(ADS1015_CONFIG_MUX_DIFF_P1_N3));
}
int16_t flxDevADS1015::readi_differential_P2_N3()
{
    return (ADS1015::getDifferential(ADS1015_CONFIG_MUX_DIFF_P2_N3));
}

// UN Singed Int params
uint16_t flxDevADS1015::readu_single_0()
{
    return (ADS1015::getSingleEnded(0));
}
uint16_t flxDevADS1015::readu_single_1()
{
    return (ADS1015::getSingleEnded(1));
}
uint16_t flxDevADS1015::readu_single_2()
{
    return (ADS1015::getSingleEnded(2));
}
uint16_t flxDevADS1015::readu_single_3()
{
    return (ADS1015::getSingleEnded(3));
}

// Static method used to determine if this device is connected

bool flxDevADS1015::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Point to Hi_Thresh Register
    uint8_t hiThreshReg = ADS1015_POINTER_HITHRESH;
    if (!i2cDriver.write(address, &hiThreshReg, 1))
        return false;
    // Read Hi_Thresh Register
    uint8_t hiThresh[2] = {0};
    if (i2cDriver.receiveResponse(address, hiThresh, 2) != 2)
        return false;
    delay(1);
    // Update Hi_Thresh Register
    uint8_t dummy[3] = {ADS1015_POINTER_HITHRESH, 0xAA, 0xAF};
    if (!i2cDriver.write(address, dummy, 3))
        return false;
    delay(1);
    // Point to Hi_Thresh Register
    if (!i2cDriver.write(address, &hiThreshReg, 1))
        return false;
    // Re-read Hi_Thresh Register
    uint8_t newHiThresh[2] = {0};
    if (i2cDriver.receiveResponse(address, newHiThresh, 2) != 2)
        return false;
    delay(1);
    // Restore Hi_Thresh Register
    uint8_t dummy2[3];
    dummy2[0] = ADS1015_POINTER_HITHRESH;
    dummy2[1] = hiThresh[0];
    dummy2[2] = hiThresh[1];
    if (!i2cDriver.write(address, dummy2, 3))
        return false;

    return ((newHiThresh[0] == 0xAA) && (newHiThresh[1] == 0xAF));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevADS1015::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    bool result = ADS1015::begin(address(), wirePort);

    if (!result)
        flxLog_E("ADS1015 - begin failed");

    if (result)
    {
        _begun = true;
        ADS1015::useConversionReady(true);

        // we are now connected - let's set sampleRate and Gain
        // Note: Saved values are restored before onInitialize() is called,
        //       so these could be something different than the defaults.
        set_sample_rate(_sampleRate);
        set_pga_gain(_gain);
    }

    return result;
}

// read-write property methods

uint16_t flxDevADS1015::get_sample_rate()
{
    if (_begun)
        _sampleRate = ADS1015::getSampleRate();
    return _sampleRate;
}
void flxDevADS1015::set_sample_rate(uint16_t rate)
{
    _sampleRate = rate;
    if (_begun)
        ADS1015::setSampleRate(rate);
}
uint16_t flxDevADS1015::get_pga_gain()
{
    if (_begun)
        _gain = ADS1015::getGain();
    return _gain;
}
void flxDevADS1015::set_pga_gain(uint16_t gain)
{
    _gain = gain;
    if (_begun)
        ADS1015::setSampleRate(gain);
}

//----------------------------------------------------------------------------------------------------------
// Property getter and setters for types ...
//----------------------------------------------------------------------------------------------------------
// Methods to adjust types

uint8_t flxDevADS1015::get_se_type(void)
{
    return _seType;
}
//----------------------------------------------------------------------------------------------------------
void flxDevADS1015::set_se_type(uint8_t inType)
{

    _seType = inType;

    bool enableF = _seType == kADS1015DeviceFloat;
    bool enableI = _seType == kADS1015DeviceInt;
    bool enableU = _seType == kADS1015DeviceUnsigned;

    channel0_f.setEnabled(enableF);
    channel1_f.setEnabled(enableF);
    channel2_f.setEnabled(enableF);
    channel3_f.setEnabled(enableF);

    channel0_i.setEnabled(enableI);
    channel1_i.setEnabled(enableI);
    channel2_i.setEnabled(enableI);
    channel3_i.setEnabled(enableI);

    channel0_u.setEnabled(enableU);
    channel1_u.setEnabled(enableU);
    channel2_u.setEnabled(enableU);
    channel3_u.setEnabled(enableU);
}

//----------------------------------------------------------------------------------------------------------
uint8_t flxDevADS1015::get_diff_type(void)
{
    return _diffType;
}

//----------------------------------------------------------------------------------------------------------
void flxDevADS1015::set_diff_type(uint8_t inType)
{

    _diffType = inType;

    bool enableF = _diffType == kADS1015DeviceFloat;
    bool enableI = _diffType == kADS1015DeviceInt;

    differential_0_minus_1_f.setEnabled(enableF);
    differential_0_minus_3_f.setEnabled(enableF);
    differential_1_minus_3_f.setEnabled(enableF);
    differential_2_minus_3_f.setEnabled(enableF);

    differential_0_minus_1_i.setEnabled(enableI);
    differential_0_minus_3_i.setEnabled(enableI);
    differential_1_minus_3_i.setEnabled(enableI);
    differential_2_minus_3_i.setEnabled(enableI);
}