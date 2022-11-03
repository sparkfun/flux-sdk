/*
 *
 *  spDevBMP384.cpp
 *
 *  Device object for the BMP384 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevBMP384.h"

#define BMP384_CHIP_ID_REG 0x00 // Chip ID 0x50

#define kBMP384AddressDefault 0x77
#define kBMP384AddressAlt1 0x76

// Define our class static variables - allocs storage for them

uint8_t spDevBMP384::defaultDeviceAddress[] = {kBMP384AddressDefault, kBMP384AddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevBMP384);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBMP384::spDevBMP384()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BMP384 Pressure and Temperature Sensor");

    // Register parameters
    spRegister(temperatureC, "Temperature (C)", "The sensed temperature in degrees Celsius");
    spRegister(pressure, "Pressure (Pa)", "The sensed pressure in Pascals");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBMP384::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = 0;
    bool couldBe384 = i2cDriver.readRegister(address, BMP384_CHIP_ID_REG, &chipID); // Should return 0x50 for BMP384
    couldBe384 &= chipID == 0x50; // Note: BMP390 returns 0x60

    return (couldBe384);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBMP384::onInitialize(TwoWire &wirePort)
{

    int8_t result = BMP384::beginI2C(address(), wirePort);

    if (result != BMP3_OK)
    {
        spLog_E("BMP384 Sensor error: %d", result);
        return false;
    }
	
    return true;
}

// GETTER methods for output params
double spDevBMP384::read_TemperatureC()
{
    if (!_temperature)
    {
        int8_t err = BMP384::getSensorData(&bmpData);

        if (err == BMP3_OK)
            _pressure = true;
	}
    _temperature = false;
    return bmpData.temperature;
}
double spDevBMP384::read_Pressure()
{
    if (!_pressure)
    {
        int8_t err = BMP384::getSensorData(&bmpData);

        if (err == BMP3_OK)
            _temperature = true;
	}
    _pressure = false;
    return bmpData.pressure;
}
