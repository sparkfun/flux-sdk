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

#include "spDevBME280.h"

#define BME280_CHIP_ID_REG		0xD0 //Chip ID
#define BME280_I2C_ADDR			0x77

spType spDevBME280::Type;
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBME280::spDevBME280(){

	// Setup unique identifiers for this device and basic device object systems
	spSetupDeviceIdent(DEVICE_NAME);

	// Register the properties with the system - this makes the connections needed
	// to support managed properties/public properties
	//
	// Set default values for the properties. Note: If stored/serialized values exist,
	// they are loaded after this method - as the system starts up. 

	spRegisterProperty(celsius, false);		


	// Setup output parameters - connect to the data method for the source.
	// Note: These methods are from the super class - the original qwiic library.
	spSetupOutParameter(temperature_f, spDevBME280::readTempF);
	spSetupOutParameter(temperature_c, spDevBME280::readTempC);	
	spSetupOutParameter(humidity, spDevBME280::readFloatHumidity);
	spSetupOutParameter(pressure, spDevBME280::readFloatPressure);		

}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBME280::isConnected(spDevI2C& i2cDriver){

	uint8_t chipID = i2cDriver.readRegister(BME280_I2C_ADDR, BME280_CHIP_ID_REG); //Should return 0x60 or 0x58

	return (chipID == 0x58 || chipID == 0x60);

}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBME280::onInitialize(TwoWire& wirePort){

	return BME280::beginI2C(wirePort);

}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated. 
//
void spDevBME280::onPropertyUpdate(const char * propName){

	//Serial.print("PROPERTY UPDATE: ");Serial.println(propName);
	//save();
}

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system 
// initialization and device discovery. 


spRegisterDevice(spDevBME280);


