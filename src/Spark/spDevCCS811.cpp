/*
 *
 *  spDevCCS811.cpp
 *
 *  Spark Device object for the CCS811 Qwiic device.
 */

#include "Arduino.h"

#include "spDevCCS811.h"


// For Device detection - chip ID things
#define kCCS811ChipIdReg 0x20 
#define kCCS811ChipIdValue 0x81

#define kCCS811AddressDefault 0x5B
#define kCCS811AddressAlt1 0x5A

uint8_t spDevCCS811::defaultDeviceAddress[] = { kCCS811AddressDefault, 
												kCCS811AddressAlt1, 
												kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevCCS811);

// KDB NOTE: Need to check in ccs arduino lib code updates

spDevCCS811::spDevCCS811() //: CCS811(kCCS811AddressDefault)
{

    setName(getDeviceName());
    setDescription("An air quality sensor from AMS");

    // Register output params
    spRegister(co2, "CO2", "CO2 reading");
    spRegister(tvoc, "VOC", "Volatile Organic Compound reading");    

}

// Function to encapsulate the ops needed to get values from the sensor.
// Shouldn't this be part of the original library?
float spDevCCS811::read_CO2()
{

    if (_co2 == false)
    {
        if (CCS811::dataAvailable())
        {
            CCS811::readAlgorithmResults();
            _tvoc = true;
        }
    }
    _co2 = false;

    // Call the supers method
    // If !dataAvailable, getCO2 returns the previous value. Do this instead of returning 0.0.
    return CCS811::getCO2();
}
//
float spDevCCS811::read_TVOC()
{
    if (_tvoc == false)
    {
        if (CCS811::dataAvailable())
        {
            CCS811::readAlgorithmResults();
            _co2 = true;
        }
    }
    _tvoc = false;

    // Call the supers method
    // If !dataAvailable, getTVOC returns the previous value. Do this instead of returning 0.0.
    return CCS811::getTVOC();
}

// Static method used to determine if this device is connected

bool spDevCCS811::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = i2cDriver.readRegister(address, kCCS811ChipIdReg); 

    return (chipID == kCCS811ChipIdValue);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevCCS811::onInitialize(TwoWire &wirePort)
{
    _tvoc = false; // Flags to avoid calling readAlgorithmResults twice
    _co2 = false;

	// set the underlying drivers address to the one determined during
	// device construction
    CCS811::setI2CAddress(address());

    return CCS811::begin(wirePort);
}
