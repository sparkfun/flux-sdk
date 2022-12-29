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
uint8_t flxDevADS1015::defaultDeviceAddress[] = { 0x48, 0x49, 0x4A, 0x4B, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevADS1015);

flxDevADS1015::flxDevADS1015()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("ADS1015 A/D Converter");

    // Register Properties

    flxRegister(sampleRate, "Sample Rate", "The sample rate in Hz");    
    flxRegister(gain, "Gain", "Programmable gain setting");

    // Register output params
    flxRegister(channel0, "Channel 0 Single-Ended (mV)", "Channel 0 Single-Ended (millivolts)");
    flxRegister(channel1, "Channel 1 Single-Ended (mV)", "Channel 1 Single-Ended (millivolts)");
    flxRegister(channel2, "Channel 2 Single-Ended (mV)", "Channel 2 Single-Ended (millivolts)");
    flxRegister(channel3, "Channel 3 Single-Ended (mV)", "Channel 3 Single-Ended (millivolts)");
    flxRegister(differential_0_minus_1, "Differential: P0 N1 (mV)", "Differential: Channel 0 - Channel 1 (millivolts)");
    flxRegister(differential_0_minus_3, "Differential: P0 N3 (mV)", "Differential: Channel 0 - Channel 3 (millivolts)");
    flxRegister(differential_1_minus_3, "Differential: P1 N3 (mV)", "Differential: Channel 1 - Channel 3 (millivolts)");
    flxRegister(differential_2_minus_3, "Differential: P2 N3 (mV)", "Differential: Channel 2 - Channel 3 (millivolts)");
}

// Function to encapsulate the ops needed to get values from the sensor.
float flxDevADS1015::read_single_0()
{
    return (ADS1015::getSingleEndedMillivolts(0));
}
float flxDevADS1015::read_single_1()
{
    return (ADS1015::getSingleEndedMillivolts(1));
}
float flxDevADS1015::read_single_2()
{
    return (ADS1015::getSingleEndedMillivolts(2));
}
float flxDevADS1015::read_single_3()
{
    return (ADS1015::getSingleEndedMillivolts(3));
}
float flxDevADS1015::read_differential_P0_N1()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P0_N1));
}
float flxDevADS1015::read_differential_P0_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P0_N3));
}
float flxDevADS1015::read_differential_P1_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P1_N3));
}
float flxDevADS1015::read_differential_P2_N3()
{
    return (ADS1015::getDifferentialMillivolts(ADS1015_CONFIG_MUX_DIFF_P2_N3));
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
    uint8_t dummy[3] = { ADS1015_POINTER_HITHRESH, 0xAA, 0xAF };
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
