/*
 *
 *  spDevVEML7700.h
 *
 *  Spark Device object for the VEML7700 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevVEML7700.h"

// Define our class static variables - allocs storage for them

#define kVEML7700AddressDefault VEML7700_I2C_ADDRESS

uint8_t spDevVEML7700::defaultDeviceAddress[] = {kVEML7700AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevVEML7700);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevVEML7700::spDevVEML7700()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VEML7700 Ambient Light Sensor");

    // Register parameters
    spRegister(ambientLight, "Ambient Light Level", "Ambient Light Level");
    spRegister(whiteLevel, "White Level", "White Level");
    spRegister(lux, "Lux", "Lux");

    // Register read-write properties
    spRegister(integrationTime, "Integration Time", "Integration Time: 0 = 25ms; 5 = 800ms");
    spRegister(sensitivity, "Sensitivity", "Sensitivity : 0 = x1; 1 = x2; 2 = x1/8; 3 = x1/4");
    spRegister(persistence, "Persistence", "Persistence : 0 = x1; 1 = x2; 2 = x4; 3 = x8");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevVEML7700::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t configReg;
    if (!i2cDriver.readRegister16(address, 0, &configReg, true)) // Little Endian
        return false;

    return ((configReg & 0b1110010000001100) == 0); // Check bits 15-13, 10 and 3-2 are zero
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevVEML7700::onInitialize(TwoWire &wirePort)
{

    return VEML7700::begin(wirePort);
}

// GETTER methods for output params
uint spDevVEML7700::read_ambient_light()
{
    return VEML7700::getAmbientLight();
}

uint spDevVEML7700::read_white_level()
{
    return VEML7700::getWhiteLevel();
}

float spDevVEML7700::read_lux()
{
    return VEML7700::getLux();
}

// methods for read-write properties
uint spDevVEML7700::get_integration_time()
{
    return VEML7700::getIntegrationTime();
}

void spDevVEML7700::set_integration_time(uint intTime)
{
    VEML7700::setIntegrationTime((VEML7700_integration_time_t)intTime);
}

uint spDevVEML7700::get_sensitivity()
{
    return VEML7700::getSensitivityMode();
}

void spDevVEML7700::set_sensitivity(uint sens)
{
    VEML7700::setSensitivityMode((VEML7700_sensitivity_mode_t)sens);
}

uint spDevVEML7700::get_persistence()
{
    return VEML7700::getPersistenceProtect();
}

void spDevVEML7700::set_persistence(uint pers)
{
    VEML7700::setPersistenceProtect((VEML7700_persistence_protect_t)pers);
}

