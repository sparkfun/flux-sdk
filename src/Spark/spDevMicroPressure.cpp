/*
 *
 *  spDevMicroPressure.cpp
 *
 *  Spark Device object for the MicroPressure device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevMicroPressure.h"

// The default address is named DEFAULT_ADDRESS... Let's not use that...
uint8_t spDevMicroPressure::defaultDeviceAddress[] = { 0x18, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevMicroPressure);

spDevMicroPressure::spDevMicroPressure()
{

    spSetupDeviceIdent(getDeviceName());

    // Register output params
    spRegister(pressure_PSI, "Pressure (PSI)", "Pressure (PSI)");
    spRegister(pressure_Pa, "Pressure (Pa)", "Pressure (Pa)");
    spRegister(pressure_kPa, "Pressure (kPa)", "Pressure (kPa)");
    spRegister(pressure_torr, "Pressure (torr)", "Pressure (torr)");
    spRegister(pressure_inHg, "Pressure (inHg)", "Pressure (inHg)");
    spRegister(pressure_atm, "Pressure (atm)", "Pressure (atm)");
    spRegister(pressure_bar, "Pressure (bar)", "Pressure (bar)");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevMicroPressure::read_pressure_PSI()
{
    return SparkFun_MicroPressure::readPressure();
}
float spDevMicroPressure::read_pressure_Pa()
{
    return SparkFun_MicroPressure::readPressure(PA);
}
float spDevMicroPressure::read_pressure_kPa()
{
    return SparkFun_MicroPressure::readPressure(KPA);
}
float spDevMicroPressure::read_pressure_torr()
{
    return SparkFun_MicroPressure::readPressure(TORR);
}
float spDevMicroPressure::read_pressure_inHg()
{
    return SparkFun_MicroPressure::readPressure(INHG);
}
float spDevMicroPressure::read_pressure_atm()
{
    return SparkFun_MicroPressure::readPressure(ATM);
}
float spDevMicroPressure::read_pressure_bar()
{
    return SparkFun_MicroPressure::readPressure(BAR);
}

// Static method used to determine if this device is connected

bool spDevMicroPressure::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the sensor status
    uint8_t status = 0;
    int response = i2cDriver.receiveResponse(address, &status, 1);

    return ((response == 1) && ((status & 0x5A) == 0x40));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMicroPressure::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = SparkFun_MicroPressure::begin(address(), wirePort);

    if (!result)
        spLog_E("MicroPressure - begin failed");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevMicroPressure::onPropertyUpdate(const char *propName)
{
}

