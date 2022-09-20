/*
 *
 * QwiicDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevMCP9600.h"


#define kMCPAddressDefault 0x60

// Define our class static variables - allocs storage for them

spType spDevMCP9600::Type;
uint8_t spDevMCP9600::defaultDeviceAddress[] = {kMCPAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevMCP9600);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevMCP9600::spDevMCP9600()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(getDeviceName());




    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties
    //
    // Set default values for the properties. Note: If stored/serialized values exist,
    // they are loaded after this method - as the system starts up.

    spRegisterProperty(ambient_resolution, false);
    spRegisterProperty(thermocouple_resolution, 0);
    spRegisterProperty(thermocouple_type, 0);
    spRegisterProperty(filter_coefficent, 0);        

    // Setup output parameters - connect to the data method for the source.
    // Note: These methods are from the super class - the original qwiic library.
    spSetupOutParameter(thermocouple_temp, spDevMCP9600::_getThermocoupleTemp);
    spSetupOutParameter(ambient_temp, spDevMCP9600::_getAmbientTemp);
    spSetupOutParameter(temp_delta, spDevMCP9600::_getTempDelta);
    spSetupOutParameter(raw_adc, MCP9600::getRawADC);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevMCP9600::isConnected(spDevI2C &i2cDriver, uint8_t address)
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
bool spDevMCP9600::onInitialize(TwoWire &wirePort)
{

    // set the device address
    return MCP9600::begin(address(), wirePort);
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevMCP9600::onPropertyUpdate(const char *propName)
{

    // Serial.print("PROPERTY UPDATE: ");Serial.println(propName);
    // save();
}

