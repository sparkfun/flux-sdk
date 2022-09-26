/*
 *
 * spDevBME280.cpp
 *
 *  Spark Device object for the BME280 Qwiic device.
 */

#include "Arduino.h"

#include "spDevCCS811.h"


// For Device detection - chip ID things
#define kCCS811ChipIdReg 0x20 
#define kCCS811ChipIdValue 0x81

#define kCCS811AddressDefault 0x5B
#define kCCS811AddressAlt1 0x5A

spType spDevCCS811::Type;
uint8_t spDevCCS811::defaultDeviceAddress[] = { kCCS811AddressDefault, 
												kCCS811AddressAlt1, 
												kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevCCS811);

// KDB NOTE: Need to check in ccs arduino lib code updates

spDevCCS811::spDevCCS811() //: CCS811(kCCS811AddressDefault)
{

    spSetupDeviceIdent(getDeviceName());

    // Register output params
    spRegister(co2, "CO2", "CO2 reading");
    spRegister(tvoc, "VOC", "Volatile Organic Compound reading");    

}

// Function to encapsualte the ops needed to ge tvalues form the sensor.
// Shouldnt' this be part of the original library?
float spDevCCS811::get_CO2()
{

    if (!dataAvailable())
    {
        return 0.0;
    }

    readAlgorithmResults();

    // call the supers method
    return CCS811::getCO2();
}
//
float spDevCCS811::get_TVOC()
{

    if (!CCS811::dataAvailable())
        return 0.0;

    readAlgorithmResults();

    // Call the supers method
    return CCS811::getTVOC();
}

// Static method used to determine if this device is connected

bool spDevCCS811::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

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
	// set the underlying drivers address to the one determined during
	// device construction
    CCS811::setI2CAddress(address());

    return CCS811::begin(wirePort);
}
