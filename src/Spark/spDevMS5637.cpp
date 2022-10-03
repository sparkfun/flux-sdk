/*
 *
 *  spDevMS5637.cpp
 *
 *  Spark Device object for the MS5637 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevMS5637.h"

uint8_t spDevMS5637::defaultDeviceAddress[] = { MS5637_ADDR, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevMS5637);

spDevMS5637::spDevMS5637()
{

    spSetupDeviceIdent(getDeviceName());

    // Register output params
    spRegister(pressure_mbar, "Pressure (mbar)", "Pressure (mbar)");
    spRegister(temperatureC, "Temperature (C)", "Temperature (C)");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevMS5637::read_pressure_mbar()
{
    return MS5637::getPressure();
}
float spDevMS5637::read_temperature_C()
{
    return MS5637::getTemperature();
}

// Static method used to determine if this device is connected

bool spDevMS5637::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // HACK: we need to disambiguate the MS5637 (PT) from the MS8607 (PHT)
    // Check for a response on 0x40 = MS8607 humidity sensor address
    // If ping(0x40) returns true then return false
    // Note: 0x40 is also a valid address for the ADS122C04...
    if (i2cDriver.ping(0x40))
        return false;

    return true;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMS5637::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = MS5637::begin(wirePort);

    if (!result)
        spLog_E("MS5637 - begin failed");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevMS5637::onPropertyUpdate(const char *propName)
{
}

