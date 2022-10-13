/*
 *
 *  spDevBME280.cpp
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
    setName(getDeviceName());
    setDescription("The Bosch BME280 Atmospheric Sensor.");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    spRegister(celsius, "Celsius", "Use celsius for temperature units");
    celsius = false;
    spRegister(metres, "Metres", "Use metres for altitude units");
    metres = false;

    // Register parameters
    spRegister(humidity, "Humidity", "The sensed humidity value");
    // spRegister(temperature);
    spRegister(temperatureF, "TemperatureF", "The sensed Temperature in degrees Fahrenheit");
    spRegister(temperatureC, "TemperatureC", "The sensed Temperature in degrees Celsius");
    spRegister(pressure, "Pressure", "The sensed pressure");
    // spRegister(altitude);
    spRegister(altitudeM, "AltitudeM", "The sensed altitude in meters");
    spRegister(altitudeF, "AltitudeF", "The sensed altitude in feet");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBME280::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

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
float spDevBME280::read_Humidity()
{
    return BME280::readFloatHumidity();
}

float spDevBME280::read_Temperature()
{
    if (celsius)
        return read_TemperatureC();
    else
        return read_TemperatureF();
}
float spDevBME280::read_TemperatureF()
{
    return BME280::readTempF();
}
float spDevBME280::read_TemperatureC()
{
    return BME280::readTempC();
}

float spDevBME280::read_Pressure()
{
    return BME280::readFloatPressure();
}

float spDevBME280::read_Altitude()
{
    if (metres)
        return read_AltitudeM();
    else
        return read_AltitudeF();
}
float spDevBME280::read_AltitudeM()
{
    return BME280::readFloatAltitudeMeters();
}
float spDevBME280::read_AltitudeF()
{
    return BME280::readFloatAltitudeFeet();
}
