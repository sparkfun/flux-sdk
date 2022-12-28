/*
 *
 *  flxDevTMP117.h
 *
 *  Spark Device object for the TMP117 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevTMP117.h"

// Define our class static variables - allocs storage for them

uint8_t flxDevTMP117::defaultDeviceAddress[] = {0x48, 0x49, 0x4A, 0x4B, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevTMP117);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevTMP117::flxDevTMP117()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("TMP117 Precision Temperature Sensor");

    // Register parameters
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevTMP117::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t deviceID;
    if (!i2cDriver.readRegister16(address, TMP117_DEVICE_ID, &deviceID, false)) // Big Endian
        return false;

    return ((deviceID & 0xFFF) == DEVICE_ID_VALUE);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevTMP117::onInitialize(TwoWire &wirePort)
{

    bool result = TMP117::begin(address(), wirePort);
    if (result)
    {
        TMP117::setConversionAverageMode(0);
        TMP117::setConversionCycleBit(0);
        TMP117::setContinuousConversionMode();
    }
    return result;
}

// GETTER methods for output params
double flxDevTMP117::read_temperature_C()
{
    return (TMP117::readTempC());
}
