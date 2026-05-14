/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevVEML6030.cpp
 *
 *  Spark Device object for the VEML6030 Ambient Light Sensor.
 *
 */

#include "Arduino.h"

#include "flxDevVEML6030.h"

// Default address 0x48 (ADDR pin low). Alternate 0x10 (ADDR pin high) is
// omitted here because the VEML7700 driver claims 0x10 with exact confidence
// and the two sensors cannot be distinguished by register reads alone.
uint8_t flxDevVEML6030::defaultDeviceAddress[] = {0x48, kSparkDeviceAddressNull};

// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevVEML6030);

// Gain index → float value expected by the library
static const float kGainValues[] = {1.0f, 2.0f, 0.125f, 0.25f};

//----------------------------------------------------------------------------------------------------------
// Constructor
flxDevVEML6030::flxDevVEML6030()
{
    setName(getDeviceName());
    setDescription("VEML6030 Ambient Light Sensor");

    flxRegister(ambientLight, "Ambient Light (lux)", "Ambient light channel in lux", kParamValueAmbientLight);
    flxRegister(whiteLight, "White Light (lux)", "White light channel in lux", kParamValueWhiteLight);

    flxRegister(gain, "Gain", "Sensor gain setting");
    flxRegister(integrationTime, "Integration Time (ms)", "Measurement integration time in milliseconds");
}

//----------------------------------------------------------------------------------------------------------
// isConnected()
//
// Heuristic check: ping the address then verify the reserved bits in the
// VEML6030 configuration register (reg 0x00) are zero.  No device-ID register
// exists, so this returns fuzzy confidence (see connectedConfidence()).
bool flxDevVEML6030::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    if (!i2cDriver.ping(address))
        return false;

    // Config register 0x00, little-endian 16-bit.
    // Bits [15:13], [10], and [3] are reserved and must read as zero.
    uint16_t configReg = 0;
    if (!i2cDriver.readRegister16(address, 0x00, &configReg, true))
        return false;

    return ((configReg & 0b1110010000001000) == 0);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
bool flxDevVEML6030::onInitialize(TwoWire &wirePort)
{
    _sensor.reset(new SparkFun_Ambient_Light(address()));

    bool ok = _sensor->begin(wirePort);
    if (ok)
    {
        // Apply any properties that were set before initialization.
        _sensor->setGain(kGainValues[_gain < 4 ? _gain : 0]);
        _sensor->setIntegTime(_integrationTime);
    }
    return ok;
}

//----------------------------------------------------------------------------------------------------------
// Output parameter readers

uint32_t flxDevVEML6030::read_ambient_light()
{
    return _sensor ? _sensor->readLight() : 0;
}

uint32_t flxDevVEML6030::read_white_light()
{
    return _sensor ? _sensor->readWhiteLight() : 0;
}

//----------------------------------------------------------------------------------------------------------
// Property getters/setters

uint8_t flxDevVEML6030::get_gain()
{
    if (isInitialized() && _sensor)
    {
        float f = _sensor->readGain();
        if (f == 1.0f)
            _gain = 0;
        else if (f == 2.0f)
            _gain = 1;
        else if (f == 0.125f)
            _gain = 2;
        else if (f == 0.25f)
            _gain = 3;
    }
    return _gain;
}

void flxDevVEML6030::set_gain(uint8_t idx)
{
    _gain = idx;
    if (isInitialized() && _sensor)
        _sensor->setGain(kGainValues[_gain < 4 ? _gain : 0]);
}

uint16_t flxDevVEML6030::get_integration_time()
{
    if (isInitialized() && _sensor)
        _integrationTime = _sensor->readIntegTime();
    return _integrationTime;
}

void flxDevVEML6030::set_integration_time(uint16_t ms)
{
    _integrationTime = ms;
    if (isInitialized() && _sensor)
        _sensor->setIntegTime(_integrationTime);
}
