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
 *  flxDevAS7343.cpp
 *
 *  Spark Device object for the AS7343  Qwiic device.
 */

#include "Arduino.h"

#include "flxDevAS7343.h"

// device addresses for our device interface -- using macros from qwiic/arduino library
uint8_t flxDevAS7343::defaultDeviceAddress[] = {kAS7343Addr, kSparkDeviceAddressNull};

///
/// @brief Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevAS7343);

//----------------------------------------------------------------------------------------------------------
/// @brief Constructor
///
flxDevAS7343::flxDevAS7343() : _gain{4}, _read_spectra{true}, _flicker_detect{false}, _valid_data{false}
{

    setName(getDeviceName(), "AS7343 Spectral Sensor");

    // Properties
    flxRegister(readWithLED, "Enable LED", "Measure with LED enabled");
    flxRegister(readSpectra, "Read Spectra", "Read the spectral data from the sensor");
    flxRegister(flickerDetect, "Flicker Detect", "Enable flicker detection");
    flxRegister(sensorGain, "Gain", "Gain settings for sensor");

    // Data parameters
    flxRegister(blueValue, "Blue", "Blue channel value");
    flxRegister(greenValue, "Green", "Green channel value");
    flxRegister(redValue, "Red", "Red channel value");
    flxRegister(nirValue, "NIR", "Near Infrared channel value");
    flxRegister(flickerValue, "Flicker", "Flicker detection value");
    flxRegister(spectralData, "Spectral Data", "All the spectral data from the sensor");
}

//----------------------------------------------------------------------------------------------------------
/// @brief  Static method called to determine if device is connected
///
/// @param  i2cDriver - Framework i2c bus driver
/// @param  address - The address to check
///
/// @return true if the device is connected
///
bool flxDevAS7343::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (i2cDriver.ping(address))
    {
        uint8_t devID; // Create a variable to hold the device ID.

        // this device has different register banks, so we need to set the bank
        sfe_as7343_reg_cfg0_t cfg0; // Create a register structure for CFG0

        // Read the cfg0 register (to retain other bits), if it errors then return 0.
        if (!i2cDriver.readRegister(address, ksfAS7343RegCfg0, &cfg0.byte))
            return false;

        cfg0.reg_bank = 1;

        // Write the cfg0 register to the device. If it errors, then return 0.
        if (!i2cDriver.writeRegister(address, ksfAS7343RegCfg0, cfg0.byte))
            return false;

        // Read the device ID register, if it errors then return 0.
        if (!i2cDriver.readRegister(address, ksfAS7343RegID, &devID))
            return 0;

        return devID == kDefaultAS7343DeviceID;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------
///
/// @brief Called during the startup/initialization of the driver (after the constructor is called).
///
/// @param wirePort - The Arduino wire port for the I2C bus
///
/// @return true on success
///
bool flxDevAS7343::onInitialize(TwoWire &wirePort)
{
    // Initialize sensor and run default setup.
    if (!SfeAS7343ArdI2C::begin())
    {
        flxLog_D(F("%s : Failed to begin sensor."), name());
        return false;
    }
    if (!SfeAS7343ArdI2C::powerOn())
    {
        flxLog_D(F("%s : Failed to power on sensor."), name());
        return false;
    }

    // Set the AutoSmux to output all 18 channels
    if (!SfeAS7343ArdI2C::setAutoSmux(AUTOSMUX_18_CHANNELS))
    {
        flxLog_D(F("%s : Failed to set AutoSmux to 18 channels."), name());
        return false;
    }

    if (_read_spectra)
    {
        // Enable Spectral Measurement
        if (!SfeAS7343ArdI2C::enableSpectralMeasurement())
        {
            flxLog_D(F("%s : Failed to enable spectral measurement."), name());
            return false;
        }
    }
    if (_flicker_detect)
    {
        // Enable Flicker Detection
        if (!SfeAS7343ArdI2C::enableFlickerDetection())
        {
            flxLog_D(F("%s : Failed to enable flicker detection."), name());
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
// Gain property - getter/setter
//---------------------------------------------------------------------------

uint8_t flxDevAS7343::get_gain(void)
{
    return _gain;
}

void flxDevAS7343::set_gain(uint8_t value)
{
    _gain = value;
    if (!SfeAS7343ArdI2C::setAgain((sfe_as7343_again_t)_gain))
        flxLog_W(F("%s : Failed to set gain to %d."), name(), _gain);
}
//---------------------------------------------------------------------------
// Read Spectra data
bool flxDevAS7343::get_read_spectra(void)
{
    return _read_spectra;
}
void flxDevAS7343::set_read_spectra(bool new_value)
{
    if (new_value == _read_spectra)
        return; // no change

    bool status = SfeAS7343ArdI2C::enableSpectralMeasurement(new_value);
    if (!status)
    {
        flxLog_W(F("%s : Failed to %s spectral measurement"), name(), new_value ? "enable" : "disable");
        return;
    }
    _read_spectra = new_value;
}
//---------------------------------------------------------------------------
// Flicker detect?
bool flxDevAS7343::get_flicker_detect(void)
{
    return _flicker_detect;
}
void flxDevAS7343::set_flicker_detect(bool new_value)
{
    if (_flicker_detect == new_value)
        return; // no change

    bool status = SfeAS7343ArdI2C::enableFlickerDetection(new_value);
    if (!status)
    {
        flxLog_W(F("%s : Failed to %s flicker detection"), name(), new_value ? "enable" : "disable");
        return;
    }
    _flicker_detect = new_value;
}

//---------------------------------------------------------------------------
///
/// @brief Called right before data parameters are read - take measurements called
///

bool flxDevAS7343::execute(void)
{

    // LED ON?
    if (readWithLED() && _read_spectra)
    {
        SfeAS7343ArdI2C::ledOn();
        delay(100);
    }

    // Read the spectra data from the sensor - this is cached by the local library.
    if (!SfeAS7343ArdI2C::readSpectraDataFromSensor())
    {
        flxLog_E(F("%s : Failed to read spectra data from sensor."), name());

        _valid_data = false;
        return false;
    }
    // LED Off
    if (readWithLED() && _read_spectra)
        SfeAS7343ArdI2C::ledOff();

    _valid_data = true;
    return true;
}

//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------
// GETTER methods for output params
bool flxDevAS7343::get_spectral_data(flxDataArrayUInt16 *outData)
{
    if (!_valid_data)
    {
        flxLog_W(F("%s : No valid spectral data available."), name());
        return false;
    }

    // array for our data
    uint16_t theData[ksfAS7343NumChannels] = {0};
    uint16_t channelsRead = SfeAS7343ArdI2C::getData(theData);
    outData->set(theData, channelsRead);

    return true;
}

uint8_t flxDevAS7343::get_flicker_value(void)
{
    if (_valid_data || !_flicker_detect)
        return 0;

    if (!SfeAS7343ArdI2C::isFlickerDetectionValid())
        return 0;
    if (SfeAS7343ArdI2C::isFlickerDetectionSaturated())
        return 0; // Get the flicker detection saturation status
    return SfeAS7343ArdI2C::getFlickerDetectionFrequency();
}