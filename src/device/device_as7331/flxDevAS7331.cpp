/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevAS7331.cpp
 *
 *  Spark Device object for the AS7331 UV Spectral Sensor (UVA/UVB/UVC).
 */

#include "Arduino.h"

#include "flxDevAS7331.h"

// Device addresses — AS7331 supports four addresses via A1/A0 pins
uint8_t flxDevAS7331::defaultDeviceAddress[] = {0x74, 0x75, 0x76, 0x77, kSparkDeviceAddressNull};

///
/// @brief Register this class with the system — this enables the *auto load* of this device
flxRegisterDevice(flxDevAS7331);

//----------------------------------------------------------------------------------------------------------
/// @brief Constructor
///
flxDevAS7331::flxDevAS7331() : _gain{GAIN_256}, _convTime{TIME_64MS}, _valid_data{false}, _in_setup{false}
{

    setName(getDeviceName(), "AS7331 UV Spectral Sensor");

    // Properties
    flxRegister(sensorGain, "Gain", "Sensor gain setting");
    flxRegister(conversionTime, "Conversion Time", "Integration/conversion time");

    // Data parameters
    flxRegister(uvaValue, "UVA", "UVA irradiance (uW/cm2)");
    flxRegister(uvbValue, "UVB", "UVB irradiance (uW/cm2)");
    flxRegister(uvcValue, "UVC", "UVC irradiance (uW/cm2)");
    flxRegister(temperatureC, "Temperature", "Sensor temperature (C)");
}

//----------------------------------------------------------------------------------------------------------
/// @brief  Static method called to determine if device is connected
///
/// @param  i2cDriver - Framework i2c bus driver
/// @param  address - The address to check
///
/// @return true if the device is connected
///
/// @note   The AS7331 powers up in configuration mode. The AGEN register (0x02)
///         contains the device ID in the upper nibble. Expected full value is 0x21
///         (devid=0x2, mutation=0x1). This distinguishes the AS7331 from BME280/BMP384
///         which share addresses 0x76/0x77.
///
bool flxDevAS7331::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // Ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // The AS7331 uses 16-bit register reads. Read AGEN register (0x02) which contains
    // the device ID. On power-up the device is in configuration mode, so this register
    // is accessible without mode switching.
    uint16_t agenValue;
    if (!i2cDriver.readRegister16(address, 0x02, &agenValue, true)) // Little Endian
        return false;

    // Low byte contains the AGEN value: upper nibble = device ID (0x2), lower = mutation (0x1)
    // Expected: 0x21
    return (agenValue & 0xFF) == 0x21;
}

//----------------------------------------------------------------------------------------------------------
///
/// @brief Called during the startup/initialization of the driver (after the constructor is called).
///
/// @param wirePort - The Arduino wire port for the I2C bus
///
/// @return true on success
///
bool flxDevAS7331::onInitialize(TwoWire &wirePort)
{

    // Initialize the sensor
    if (!SfeAS7331ArdI2C::begin())
    {
        flxLog_D(F("%s : Failed to begin sensor."), name());
        return false;
    }

    _in_setup = true;

    // Apply stored gain and conversion time before preparing measurement
    set_gain(_gain);
    set_conv_time(_convTime);

    _in_setup = false;

    // Use command (one-shot) mode — take a measurement each time execute() is called
    if (!SfeAS7331ArdI2C::prepareMeasurement(MEAS_MODE_CMD))
    {
        flxLog_D(F("%s : Failed to prepare measurement."), name());
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// Gain property - getter/setter
//---------------------------------------------------------------------------

uint16_t flxDevAS7331::get_gain(void)
{
    return _gain;
}

void flxDevAS7331::set_gain(uint16_t value)
{
    if (value == _gain && !_in_setup)
        return; // no change

    _gain = value;

    if (!isInitialized() && !_in_setup)
        return;

    if (SfeAS7331ArdI2C::setGain((as7331_gain_t)_gain) != 0)
        flxLog_W(F("%s : Failed to set gain."), name());
}

//---------------------------------------------------------------------------
// Conversion time property - getter/setter
//---------------------------------------------------------------------------

uint16_t flxDevAS7331::get_conv_time(void)
{
    return _convTime;
}

void flxDevAS7331::set_conv_time(uint16_t value)
{
    if (value == _convTime && !_in_setup)
        return; // no change

    _convTime = value;

    if (!isInitialized() && !_in_setup)
        return;

    if (SfeAS7331ArdI2C::setConversionTime((as7331_conv_time_t)_convTime) != 0)
        flxLog_W(F("%s : Failed to set conversion time."), name());
}

//---------------------------------------------------------------------------
///
/// @brief Called right before data parameters are read — triggers a one-shot measurement
///
bool flxDevAS7331::execute(void)
{
    // Start a one-shot measurement
    if (SfeAS7331ArdI2C::setStartState(true) != 0)
    {
        flxLog_E(F("%s : Failed to start measurement."), name());
        _valid_data = false;
        return false;
    }

    // Wait for conversion to complete — conversion time + margin
    delay(2 + SfeAS7331ArdI2C::getConversionTimeMillis());

    // Read all UV channels + temperature
    if (SfeAS7331ArdI2C::readAllUV() != 0)
    {
        flxLog_E(F("%s : Failed to read UV data."), name());
        _valid_data = false;
        return false;
    }

    SfeAS7331ArdI2C::readTemp();

    _valid_data = true;
    return true;
}
