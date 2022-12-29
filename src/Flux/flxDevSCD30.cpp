/*
 *
 *  flxDevSCD30.h
 *
 *  Spark Device object for the SCD30 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevSCD30.h"

#define kSCD30AddressDefault SCD30_ADDRESS

// Define our class static variables - allocs storage for them

uint8_t flxDevSCD30::defaultDeviceAddress[] = {kSCD30AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevSCD30);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevSCD30::flxDevSCD30()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SCD30 CO2 Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    flxRegister(measurementInterval, "Measurement Interval", "Interval between measurements (s)");
    flxRegister(altitudeCompensation, "Altitude Compensation", "Define the sensor altitude in metres above sea level");
    flxRegister(temperatureOffset, "Temperature Offset", "Define how warm the sensor is compared to ambient");

    // Register parameters
    flxRegister(co2PPM, "CO2 (PPM)", "CO2 concentration in Parts Per Million");
    flxRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    flxRegister(humidity, "Humidity (%RH)", "The relative humidity in %");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevSCD30::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t version[3]; // Two bytes plus CRC
    uint16_t versionReg = COMMAND_READ_FW_VER;
    uint8_t versionRegBytes[2] = { (uint8_t)(versionReg >> 8), (uint8_t)(versionReg & 0xFF)}; // MSB first
    if (!i2cDriver.write(address, versionRegBytes, 2))
        return false;
    delay(3);
    if (i2cDriver.receiveResponse(address, version, 3) != 3)
        return false;

    // Check CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 0; x < 2; x++)
    {
        crc ^= version[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return (crc == version[2]);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSCD30::onInitialize(TwoWire &wirePort)
{

    SCD30::useStaleData(true); // Return stale data instead of zeros
    return SCD30::begin(wirePort);
}

// GETTER methods for output params
uint flxDevSCD30::read_CO2()
{
    return SCD30::getCO2();
}

float flxDevSCD30::read_temperature_C()
{
    return SCD30::getTemperature();
}

float flxDevSCD30::read_humidity()
{
    return SCD30::getHumidity();
}

//----------------------------------------------------------------------------------------------------------
// RW Properties

uint flxDevSCD30::get_measurement_interval()
{
    return SCD30::getMeasurementInterval();
}

uint flxDevSCD30::get_altitude_compensation()
{
    return SCD30::getAltitudeCompensation();
}

float flxDevSCD30::get_temperature_offset()
{
    return SCD30::getTemperatureOffset();
}

void flxDevSCD30::set_measurement_interval(uint interval)
{
    SCD30::setMeasurementInterval(interval);
}

void flxDevSCD30::set_altitude_compensation(uint compensation)
{
    SCD30::setAltitudeCompensation(compensation);
}

void flxDevSCD30::set_temperature_offset(float offset)
{
    SCD30::setTemperatureOffset(offset);
}

