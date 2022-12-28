/*
 *
 *  spDevFS3000.cpp
 *
 *  Spark Device object for the FS3000 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevFS3000.h"


uint8_t spDevFS3000::defaultDeviceAddress[] = { FS3000_DEVICE_ADDRESS, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevFS3000);

spDevFS3000::spDevFS3000()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("FS3000 air velocity sensor");

    // Register output params
    spRegister(flow_mps, "Flow (MPS)", "Flow (Metres Per Second)");
    spRegister(flow_mph, "Flow (MPH)", "Flow (Miles Per Hour)");

    // Register property
    spRegister(fs3000version, "FS3000 Version", "FS3000 Sensor Version");
}

// Methods to read the parameters
float spDevFS3000::read_mps()
{
    return FS3000::readMetersPerSecond();
}
float spDevFS3000::read_mph()
{
    return FS3000::readMilesPerHour();
}

// Static method used to determine if this device is connected

bool spDevFS3000::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Initialize checksumData to something non-zero.
    // If checksumData is initialized to zero and the read fails, sum will also be zero and taken as valid.
    uint8_t checksumData[5] = {0xFF};
    if (i2cDriver.receiveResponse(address, checksumData, 5) != 5) // The FS3000 has no registers..
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
bool spDevFS3000::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = FS3000::begin(wirePort);

    if (!result)
        flxLog_E("FS3000 - begin failed");

    if (result)
    {
        _begun = true;
        if (_fs3000_1005)
            FS3000::setRange(AIRFLOW_RANGE_7_MPS); // FS3000-1005
        else
            FS3000::setRange(AIRFLOW_RANGE_15_MPS); // FS3000-1015
    }

    return result;
}

// Read-write properties
uint8_t spDevFS3000::get_fs3000_version() { return (uint8_t)_fs3000_1005; }
void spDevFS3000::set_fs3000_version(uint8_t is1005)
{
    _fs3000_1005 = (bool)is1005;
    if (_begun)
    {
        if (_fs3000_1005)
            FS3000::setRange(AIRFLOW_RANGE_7_MPS); // FS3000-1005
        else
            FS3000::setRange(AIRFLOW_RANGE_15_MPS); // FS3000-1015
    }
}
