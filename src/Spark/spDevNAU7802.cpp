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

#include "spDevNAU7802.h"

#define kNAU7802AddressDefault 0x2A

// Define our class static variables - allocs storage for them

uint8_t spDevNAU7802::defaultDeviceAddress[] = {kNAU7802AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevNAU7802);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevNAU7802::spDevNAU7802()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("NAU7802 Load Cell Amplifier");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    spRegister(zeroOffset, "Zero Offset", "Zero Offset");
    zeroOffset = 0;
    spRegister(calibrationFactor, "Calibration Factor", "Used to convert the scale ADU into units");
    calibrationFactor = 1.0;
    
    // Register parameters
    spRegister(weight, "Weight", "Weight in units - as set by the calibrationFactor");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevNAU7802::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t version = i2cDriver.readRegister(address, NAU7802_DEVICE_REV);
    Serial.printf("NAU7802 version 0x%02X\r\n", version);

    return true;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevNAU7802::onInitialize(TwoWire &wirePort)
{

    return NAU7802::begin(wirePort);
}

// GETTER methods for output params
float spDevNAU7802::read_weight()
{
    return NAU7802::getWeight(true); // Allow negative weights
}

uint32_t spDevNAU7802::read_zero_offset()
{
    return NAU7802::getZeroOffset();
}

float spDevNAU7802::read_calibration_factor()
{
    return NAU7802::getCalibrationFactor();
}

void spDevNAU7802::calculate_zero_offset(bool dummy)
{
    (void)dummy;

    NAU7802::calculateZeroOffset(64); // Zero the scale - calculateZeroOffset(uint8_t averageAmount = 8)
    zeroOffset = NAU7802::getZeroOffset();
}

void spDevNAU7802::calculate_calibration_factor(float weight_in_units)
{
    NAU7802::calculateCalibrationFactor(weight_in_units, 64); // Set the calibration factor - calculateCalibrationFactor(float weightOnScale, uint8_t averageAmount = 8)
    calibrationFactor = NAU7802::getCalibrationFactor();
}


//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevButton::onPropertyUpdate(const char *propName)
{

    if (strcmp(propName, "zeroOffset") == 0) // New zero offset applied
        NAU7802::setZeroOffset(zeroOffset);

    if (strcmp(propName, "calibrationFactor") == 0) // New zero offset applied
        NAU7802::setCalibrationFactor(calibrationFactor);
}

