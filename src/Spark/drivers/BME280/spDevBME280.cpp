/*
 *
 * spDevBME280.cpp
 *
 *  Spark Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevBME280.h"

#define BME280_CHIP_ID_REG 0xD0 // Chip ID

#define kBMEAddressDefault 0x77
#define kBMEAddressAlt1 0x76

// Define our class static variables - allocs storage for them

spType spDevBME280::Type;
uint8_t spDevBME280::defaultDeviceAddress[] = {kBMEAddressDefault, kBMEAddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevBME280);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBME280::spDevBME280()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(getDeviceName());

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties
    //
    // Set default values for the properties. Note: If stored/serialized values exist,
    // they are loaded after this method - as the system starts up.

    //spRegisterProperty(celsius, false);
	//spPropertySetGetter(celsius, spDevBME280::get_celsius);
	//spPropertySetSetter(celsius, spDevBME280::set_celsius);	
    // celsius.set_getter(this, &spDevBME280::get_celsius);
    // celsius.set_setter(this, &spDevBME280::set_celsius);
    celsius(this);
    // Setup output parameters - connect to the data method for the source.
    // Note: These methods are from the super class - the original qwiic library.
    spSetupOutParameter(temperature_f, spDevBME280::readTempF);
    spSetupOutParameter(temperature_c, spDevBME280::readTempC);
    spSetupOutParameter(humidity, spDevBME280::readFloatHumidity);
    spSetupOutParameter(pressure, spDevBME280::readFloatPressure);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBME280::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

    uint8_t chipID = i2cDriver.readRegister(address, BME280_CHIP_ID_REG); // Should return 0x60 or 0x58

    return (chipID == 0x58 || chipID == 0x60);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBME280::onInitialize(TwoWire &wirePort)
{

    // set the device address
    BME280::setI2CAddress(address());
    return BME280::beginI2C(wirePort);
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevBME280::onPropertyUpdate(const char *propName)
{

    // Serial.print("PROPERTY UPDATE: ");Serial.println(propName);
    // save();
}

