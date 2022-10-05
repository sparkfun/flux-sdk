/*
 *
 *  spDevLPS25HB.cpp
 *
 *  Spark Device object for the LPS25HB device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevLPS25HB.h"

uint8_t spDevLPS25HB::defaultDeviceAddress[] = { LPS25HB_I2C_ADDR_DEF, LPS25HB_I2C_ADDR_ALT, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevLPS25HB);

spDevLPS25HB::spDevLPS25HB()
{

    spSetupDeviceIdent(getDeviceName());

    // Register output params
    spRegister(temperatureC, "Temperature (C)", "Temperature (C)");
    spRegister(pressurehPa, "Pressure (hPa)", "Pressure (hPa)");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevLPS25HB::read_temperature_c()
{
    return LPS25HB::getTemperature_degC();
}
float spDevLPS25HB::read_pressure_hpa()
{
    return LPS25HB::getPressure_hPa();
}

// Static method used to determine if this device is connected

bool spDevLPS25HB::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = i2cDriver.readRegister(address, LPS25HB_REG_WHO_AM_I); 

    return (chipID == LPS25HB_DEVID);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevLPS25HB::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = LPS25HB::begin(wirePort, address());

    if (!result)
        spLog_E("LPS25HB - begin failed");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevLPS25HB::onPropertyUpdate(const char *propName)
{
}

