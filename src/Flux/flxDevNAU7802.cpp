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
 * QwiicDevNAU7802.cpp
 *
 *  Device object for the NAU7802 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevNAU7802.h"

//----------------------------------------------------------------------------------------------------------
// 02/2024 NOTE:
//     A customer reported in that the results from this device were irregular after wake/sleep events.
//     Research into this resulted in a re-evaluation and update to the underlying Arduino library.
//
//     For for this implementation, we are using an "External" calibration method. These external
//     values are calculated during the 'zero calibration' function call and stored in the hidden
//     properties `_externalCalOffset` and `_externalCalGain`, persisting these values between
//     startup/shutdown of the system AND keeping them hidden from the user.
//
//     Additionally, the startup the device is done *manually*, skipping the defaults that
//     the standard begin() call performs. This better aligns the device for the framework and
//     external calibration.
//
//
#define kNAU7802AddressDefault 0x2A

// Define our class static variables - allocs storage for them

uint8_t flxDevNAU7802::defaultDeviceAddress[] = {kNAU7802AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevNAU7802);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevNAU7802::flxDevNAU7802()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("NAU7802 Load Cell Amplifier");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    flxRegister(zeroOffset, "Zero Offset", "The scale zero offset");
    flxRegister(calibrationFactor, "Calibration Factor", "Used to convert the scale ADU into units");

    // hidden
    flxRegister(_externalCalOffset, "ExternalOffset");
    flxRegister(_externalCalGain, "ExternalGain");

    // Register parameters
    flxRegister(weight, "Weight", "Weight in units - as set by the calibrationFactor");
    flxRegister(calculateZeroOffset, "Calculate Zero Offset",
                "Perform a zero offset calibration. Sets the scale weight to zero");
    flxRegister(calculateCalibrationFactor, "Calculate Calibration Factor",
                "Perform a scale calibration. Sets the scale weight to this many units");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevNAU7802::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    // Ping it twice. The first one often fails...
    i2cDriver.ping(address);
    delay(10);
    if (!i2cDriver.ping(address))
        return false;

    uint8_t PGA = 0;
    if (!i2cDriver.readRegister(address, NAU7802_PGA, &PGA)) // REG0x1B: PGA Registers
        return false;

    return ((PGA & 0x06) == 0); // Datasheet says bits 1 and 2 should be 0
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//

bool flxDevNAU7802::onInitialize(TwoWire &wirePort)
{

    // false - skips default begin/startup calibrateAFE
    bool status = NAU7802::begin(wirePort, false);

    // Let's go?!
    if (status)
    {
        status &= reset(); // Reset all registers

        status &= powerUp(); // Power on analog and digital sections of the scale

        status &= setLDO(NAU7802_LDO_3V0); // Set LDO to 3.0V

        status &= setGain(NAU7802_GAIN_128); // Set gain to 128

        status &= setSampleRate(NAU7802_SPS_320); // Set samples per second to 320

        uint8_t adc = NAU7802::getRegister(NAU7802_ADC);
        status &= setRegister(NAU7802_ADC, (adc | 0x30)); // Turn off CLK_CHP. From 9.1 power on sequencing.

        // Enable 330pF decoupling cap on chan 2. From 9.14 application circuit note.
        status &= setBit(NAU7802_PGA_PWR_PGA_CAP_EN, NAU7802_PGA_PWR);

        // Ensure LDOMODE bit is clear - improved accuracy and higher DC gain, with ESR < 1 ohm
        status &= clearBit(NAU7802_PGA_LDOMODE, NAU7802_PGA);
        // status &= NAU7802::calibrateAFE();

        delay(getLDORampDelay()); // Wait for LDO to ramp
    }

    return status;
}

//----------------------------------------------------------------------------------------------------------
// GETTER methods for output params
float flxDevNAU7802::read_weight()
{
    return NAU7802::getWeight(true, 16); // Allow negative weights
}

//----------------------------------------------------------------------------------------------------------
// methods used to get values for our RW properties

int flxDevNAU7802::get_zero_offset()
{
    return NAU7802::getZeroOffset();
}

//----------------------------------------------------------------------------------------------------------
float flxDevNAU7802::get_calibration_factor()
{
    return NAU7802::getCalibrationFactor();
}

//----------------------------------------------------------------------------------------------------------
void flxDevNAU7802::set_zero_offset(int offset)
{
    NAU7802::setZeroOffset(offset);
}

//----------------------------------------------------------------------------------------------------------
void flxDevNAU7802::set_calibration_factor(float factor)
{
    NAU7802::setCalibrationFactor(factor);
}

//----------------------------------------------------------------------------------------------------------
int flxDevNAU7802::get_ext_offset(void)
{
    return NAU7802::get24BitRegister(NAU7802_OCAL1_B2); // Save new offset
}
//----------------------------------------------------------------------------------------------------------
void flxDevNAU7802::set_ext_offset(int offset)
{
    NAU7802::set24BitRegister(NAU7802_OCAL1_B2, offset);
    NAU7802::getWeight(true, 10); // flush
}

//----------------------------------------------------------------------------------------------------------
uint flxDevNAU7802::get_ext_gain(void)
{
    return NAU7802::get32BitRegister(NAU7802_GCAL1_B3); // This should not have changed, but read it anyway
}

//----------------------------------------------------------------------------------------------------------
void flxDevNAU7802::set_ext_gain(uint gain)
{
    set32BitRegister(NAU7802_GCAL1_B3, gain);
    NAU7802::getWeight(true, 10); // flush
}

//----------------------------------------------------------------------------------------------------------
// methods for our input parameters

void flxDevNAU7802::calculate_zero_offset()
{

    NAU7802::getWeight(true, 10);                 // flush the device
    NAU7802::calibrateAFE(NAU7802_CALMOD_OFFSET); // External offset calibration

    _externalCalOffset = NAU7802::get24BitRegister(NAU7802_OCAL1_B2); // Save new offset
    _externalCalGain = NAU7802::get32BitRegister(NAU7802_GCAL1_B3);   // This should not have changed, but read it
    // anyway

    NAU7802::calculateZeroOffset(64); // Zero the scale - calculateZeroOffset(uint8_t averageAmount = 8)

    NAU7802::getWeight(true, 10); // flush the device
    // This has changed the value of the zero offset property in the underlying driver.
    // Set the dirty flag so that system knows the property changed.
    this->setIsDirty();
}

//----------------------------------------------------------------------------------------------------------
void flxDevNAU7802::calculate_calibration_factor(const float &weight_in_units)
{
    NAU7802::calculateCalibrationFactor(weight_in_units,
                                        64); // Set the calibration factor - calculateCalibrationFactor(float
                                             // weightOnScale, uint8_t averageAmount = 8)

    NAU7802::getWeight(true, 10); // flush the device
    // This has changed the value of the cal factor property in the underlying driver.
    // Set the dirty flag so that system knows the property changed.
    this->setIsDirty();
}
