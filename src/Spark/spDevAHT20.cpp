/*
 *
 *  spDevAHT20.cpp
 *
 *  Spark Device object for the AHT20 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevAHT20.h"

uint8_t spDevAHT20::defaultDeviceAddress[] = { AHT20_DEFAULT_ADDRESS, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevAHT20);

spDevAHT20::spDevAHT20()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("AHT20 Temperature and Humidity sensor");

    // Register output params
    spRegister(temperatureC, "Temperature (C)", "Temperature (C)");
    spRegister(humidity, "Humidity (%RH)", "Humidity (%RH)");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevAHT20::read_temperature_c()
{
    return AHT20::getTemperature();
}
float spDevAHT20::read_humidity()
{
    return AHT20::getHumidity();
}

// Static method used to determine if this device is connected

bool spDevAHT20::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

    return i2cDriver.ping(address);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevAHT20::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = AHT20::begin(wirePort);

    if (!result)
        spLog_E("AHT20 - begin failed");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevAHT20::onPropertyUpdate(const char *propName)
{
}

