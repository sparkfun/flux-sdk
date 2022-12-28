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
    spRegister(zeroOffset, "Zero Offset", "The scale zero offset");
    spRegister(calibrationFactor, "Calibration Factor", "Used to convert the scale ADU into units");

    // Register parameters
    spRegister(weight, "Weight", "Weight in units - as set by the calibrationFactor");
    spRegister(calculateZeroOffset, "Calculate Zero Offset", "Perform a zero offset calibration. Sets the scale weight to zero");
    spRegister(calculateCalibrationFactor, "Calculate Calibration Factor", "Perform a scale calibration. Sets the scale weight to this many units");
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

    return NAU7802::begin(wirePort);
}

// GETTER methods for output params
float flxDevNAU7802::read_weight()
{
    return NAU7802::getWeight(true); // Allow negative weights
}

// methods used to get values for our RW properties

uint flxDevNAU7802::get_zero_offset()
{
    return NAU7802::getZeroOffset();
}

float flxDevNAU7802::get_calibration_factor()
{
    return NAU7802::getCalibrationFactor();
}

void flxDevNAU7802::set_zero_offset(uint offset)
{
    NAU7802::setZeroOffset(offset);
}

void flxDevNAU7802::set_calibration_factor(float factor)
{
    NAU7802::setCalibrationFactor(factor);
}

// methods for our input parameters

void flxDevNAU7802::calculate_zero_offset()
{
    NAU7802::calculateZeroOffset(); // Zero the scale - calculateZeroOffset(uint8_t averageAmount = 8)
}

void flxDevNAU7802::calculate_calibration_factor(const float &weight_in_units)
{
    NAU7802::calculateCalibrationFactor(weight_in_units); // Set the calibration factor - calculateCalibrationFactor(float weightOnScale, uint8_t averageAmount = 8)
}

