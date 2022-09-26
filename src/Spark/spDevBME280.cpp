/*
 *
 * QwiicDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevBME280.h"

#define BME280_CHIP_ID_REG 0xD0 // Chip ID

#define kBMEAddressDefault 0x77
#define kBMEAddressAlt1 0x76

// Define our class static variables - allocs storage for them

spType spDevBME280::Type;
uint8_t spDevBME280::defaultDeviceAddress[] = {kBMEAddressDefault, kBMEAddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevBME280);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBME280::spDevBME280()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(getDeviceName());

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    spRegister(celsius, "Celsius", "Use celsius for temperature units");
    celsius = false;

    // Register parameters
    spRegister(humidity);
    spRegister(temperatureF);    
    spRegister(temperatureC);    
    spRegister(pressure);

}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBME280::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

    uint8_t chipID = i2cDriver.readRegister(address, BME280_CHIP_ID_REG); // Should return 0x60 or 0x58

    return (chipID == 0x58 || chipID == 0x60);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBME280::onInitialize(TwoWire &wirePort)
{

    // set the device address
    BME280::setI2CAddress(address());
    return BME280::beginI2C(wirePort);
}
// GETTER methods for output params
float spDevBME280::get_Humidity()
{
    return BME280::readFloatHumidity();
}

float spDevBME280::get_TemperatureF()
{
    return BME280::readTempF();
}
float spDevBME280::get_TemperatureC()
{
    return BME280::readTempC();
}
float spDevBME280::get_Pressure()
{
    return BME280::readFloatPressure();
}