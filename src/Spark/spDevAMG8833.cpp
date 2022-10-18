/*
 *
 *  spDevAMG8833.cpp
 *
 *  Spark Device object for the AMG8833 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevAMG8833.h"


uint8_t spDevAMG8833::defaultDeviceAddress[] = { 0x69, 0x68, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevAMG8833);

spDevAMG8833::spDevAMG8833()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("AMG8833 Grid-EYE infrared array");

    // Register output params
    spRegister(deviceTemperatureC, "Device Temperature (C)", "Device Temperature (C)");

    // Register property
    spRegister(frameRate, "Frame Rate (FPS)", "Frame Rate (Frames Per Second)");
}

// Methods to read the parameters
float spDevAMG8833::read_device_temperature_C()
{
    return GRidEYE::readDeviceTemperature();
}

// Static method used to determine if this device is connected

bool spDevAMG8833::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Initialize checksumData to something non-zero.
    // If checksumData is initialized to zero and the read fails, sum will also be zero and taken as valid.
    uint8_t checksumData[5] = {0xFF};
    if (!i2cDriver.receiveResponse(address, checksumData, 5)) // The AMG8833 has no registers..
        return false;

    uint8_t sum = 0;
    for (uint8_t i = 0; i <= 4; i++)
    {
        sum += checksumData[i];
    }

    return (sum == 0x00);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevAMG8833::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = GridEYE::begin(wirePort);

    if (!result)
        spLog_E("AMG8833 - begin failed");

    if (result)
    {
        _begun = true;
    }

    return result;
}

// Read-write properties
uint8_t spDevAMG8833::get_AMG8833_version() { return (uint8_t)_AMG8833_1005; }
void spDevAMG8833::set_AMG8833_version(uint8_t is1005)
{
    _AMG8833_1005 = (bool)is1005;
    if (_begun)
    {
        if (_AMG8833_1005)
            AMG8833::setRange(AIRFLOW_RANGE_7_MPS); // AMG8833-1005
        else
            AMG8833::setRange(AIRFLOW_RANGE_15_MPS); // AMG8833-1015
    }
}
