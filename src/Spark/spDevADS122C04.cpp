/*
 *
 *  spDevADS122C04.cpp
 *
 *  Spark Device object for the ADS122C04 A/D converter device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevADS122C04.h"


// The ADS122C04 supports multiple addresses. The SparkX breakout can be set to 0x45, 0x44, 0x41, 0x40
uint8_t spDevADS122C04::defaultDeviceAddress[] = { 0x45, 0x44, 0x41, 0x40, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevADS122C04);

spDevADS122C04::spDevADS122C04()
{

    spSetupDeviceIdent(getDeviceName());

    // Register Property
    spRegister(wireMode, "Wire mode", "Wire mode: 0 = 4-wire, 1 = 3-wire, 2 = 2-wire");
    wireMode = ADS122C04_4WIRE_MODE; // Default to 4-wire mode. See SFE_ADS122C04::ADS122C04_4WIRE_MODE etc.
    spRegister(sampleRate, "Sample rate", "Sample rate: 0 = 20SPS, 6 = 1000SPS");
    sampleRate = ADS122C04_DATA_RATE_20SPS; // Default to 20 samples per second. See SFE_ADS122C04::ADS122C04_DATA_RATE_20SPS etc.

    // Register output params
    spRegister(temperatureC, "Probe temperature (C)", "Probe temperature (C)");
    spRegister(temperatureF, "Probe temperature (F)", "Probe temperature (F)");
    spRegister(internalTemperature, "Internal temperature (C)", "Internal temperature (C)");
    spRegister(rawVoltage, "Raw voltage (V)", "Raw voltage (V)");

}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevADS122C04::read_temperature_c()
{
    return SFE_ADS122C04::readPT100Centigrade();
}
float spDevADS122C04::read_temperature_f()
{
    return SFE_ADS122C04::readPT100Fahrenheit();
}
float spDevADS122C04::read_internal_temperature()
{
    return SFE_ADS122C04::readInternalTemperature(sampleRate);
}
float spDevADS122C04::read_raw_voltage()
{
    int32_t raw_v = SFE_ADS122C04::readRawVoltage(sampleRate);
    return (((float)raw_v) * 244.14e-9); // Convert to Volts
}

// Static method used to determine if this device is connected

bool spDevADS122C04::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // 0x40 clashes with the humidity sensor on the MS8607 PHT sensor...
    // We need to do something clever here to make sure we don't detect an MS8607 as an ADS122C04

    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read Config Register 2
    uint8_t readCommand = ADS122C04_READ_CMD(ADS122C04_CONFIG_2_REG);
    uint8_t configReg2 = 0;
    if (!i2cDriver.readRegister(address, readCommand, &configReg2))
        return false;
    // Enable inverted data output
    uint8_t writeCommand = ADS122C04_WRITE_CMD(ADS122C04_CONFIG_2_REG);
    if (!i2cDriver.writeRegister(address, writeCommand, (configReg2 | (1 << 4)) & ~(1 << 5)))
        return false;
    delay(1);
    // Now read configReg2 again. This time it should be the register contents followed by the inverted contents
    uint16_t configReg2inv = 0;
    if (!i2cDriver.readRegister16(address, readCommand, &configReg2inv))
        return false;
    uint8_t byte1 = (uint8_t)(configReg2inv & 0xFF);
    uint8_t byte2 = configReg2inv >> 8;
    if ( 0xFF != (byte1 ^ byte2)) // Check byte1 is the inverse of byte2 by ex-oring them together. Result should be 0xFF.
        return false;
    // Finally, restore Config Register 2
    if (!i2cDriver.writeRegister(address, writeCommand, configReg2))
        return false;
    delay(1);

    return true;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevADS122C04::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = SFE_ADS122C04::begin(address(), wirePort);

    if (!result)
        spLog_E("ADS122C04 - begin failed");

    result &= SFE_ADS122C04::configureADCmode(wireMode, sampleRate);

    return result;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevADS122C04::onPropertyUpdate(const char *propName)
{
    SFE_ADS122C04::configureADCmode(wireMode, sampleRate);
}

