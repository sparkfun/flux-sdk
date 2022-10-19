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
    spRegister(pixelTemperatures, "Pixel Temperatures (C)", "Pixel Temperatures (C)");

    // Register property
    spRegister(frameRate, "Frame Rate (FPS)", "Frame Rate (Frames Per Second)");
}

// Static method used to determine if this device is connected

bool spDevAMG8833::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the thermistor temperature. Make sure it is within bounds.
    // Not a great test, but something...
    union
    {
        int16_t signed16;
        uint16_t unsigned16;
    } signedUnsigned16;
    if (!i2cDriver.readRegister16(address, 0x0E, &signedUnsigned16.unsigned16, true)) // Little-endian
        return false;

    if(signedUnsigned16.unsigned16 >> 12) // Check the 4 unused bits are zero
        return false;

    if (signedUnsigned16.unsigned16 & (1 << 11)) // If the signed bit (bit 11) is set
        signedUnsigned16.unsigned16 |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement

    // Temperature LSB resolution is 0.0625C. -20C is -320. +60C is 960.
    return ((signedUnsigned16.signed16 > -320) && (signedUnsigned16.signed16 < 960));
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
    GridEYE::begin(address(), wirePort); // Returns void

    if (_frameRate10FPS)
        GridEYE::setFramerate10FPS();
    else
        GridEYE::setFramerate1FPS();

    bool result = GridEYE::isFramerate10FPS() == _frameRate10FPS; // Not a great test, but something...
    
    if (!result)
        spLog_E("AMG8833 - begin failed");

    if (result)
        _begun = true;

    return result;
}

// Read-write properties
uint8_t spDevAMG8833::get_frame_rate() { return (uint8_t)_frameRate10FPS; }
void spDevAMG8833::set_frame_rate(uint8_t is10FPS)
{
    _frameRate10FPS = (bool)is10FPS;
    if (_begun)
    {
        if (_frameRate10FPS)
            GridEYE::setFramerate10FPS();
        else
            GridEYE::setFramerate1FPS();
    }
}

// Output parameters
float spDevAMG8833::read_device_temperature_C()
{
    return GridEYE::getDeviceTemperature();
}

std::string spDevAMG8833::read_pixel_temperatures()
{
/*// Simple heat map

    char temps[64];

    for (uint8_t i = 0; i < 64; i++)
    {
        float temp = GridEYE::getPixelTemperature(i);
        // Convert into a simple heat map
        if (temp > 30)
            temps[i] = 'O';
        else if (temp > 20)
            temps[i] = '0';
        else if (temp > 10)
            temps[i] = 'o';
        else
            temps[i] = '.';
    }

    char szBuffer1[10] = {'\0'};
    snprintf(szBuffer1, sizeof(szBuffer1), "%c%c%c%c%c%c%c%c", temps[0x00], temps[0x01], temps[0x02], temps[0x03], temps[0x04], temps[0x05], temps[0x06], temps[0x07]);
    char szBuffer2[10] = {'\0'};
    snprintf(szBuffer2, sizeof(szBuffer2), "%c%c%c%c%c%c%c%c", temps[0x08], temps[0x09], temps[0x0A], temps[0x0B], temps[0x0C], temps[0x0D], temps[0x0E], temps[0x0F]);
    char szBuffer3[10] = {'\0'};
    snprintf(szBuffer3, sizeof(szBuffer3), "%c%c%c%c%c%c%c%c", temps[0x10], temps[0x11], temps[0x12], temps[0x13], temps[0x14], temps[0x15], temps[0x16], temps[0x17]);
    char szBuffer4[10] = {'\0'};
    snprintf(szBuffer4, sizeof(szBuffer4), "%c%c%c%c%c%c%c%c", temps[0x18], temps[0x19], temps[0x1A], temps[0x1B], temps[0x1C], temps[0x1D], temps[0x1E], temps[0x1F]);
    char szBuffer5[10] = {'\0'};
    snprintf(szBuffer5, sizeof(szBuffer5), "%c%c%c%c%c%c%c%c", temps[0x20], temps[0x21], temps[0x22], temps[0x23], temps[0x24], temps[0x25], temps[0x26], temps[0x27]);
    char szBuffer6[10] = {'\0'};
    snprintf(szBuffer6, sizeof(szBuffer6), "%c%c%c%c%c%c%c%c", temps[0x28], temps[0x29], temps[0x2A], temps[0x2B], temps[0x2C], temps[0x2D], temps[0x2E], temps[0x2F]);
    char szBuffer7[10] = {'\0'};
    snprintf(szBuffer7, sizeof(szBuffer7), "%c%c%c%c%c%c%c%c", temps[0x30], temps[0x31], temps[0x32], temps[0x33], temps[0x34], temps[0x35], temps[0x36], temps[0x37]);
    char szBuffer8[10] = {'\0'};
    snprintf(szBuffer8, sizeof(szBuffer8), "%c%c%c%c%c%c%c%c", temps[0x38], temps[0x39], temps[0x3A], temps[0x3B], temps[0x3C], temps[0x3D], temps[0x3E], temps[0x3F]);

    char szBuffer[80] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%s,%s,%s,%s,%s,%s,%s,%s", szBuffer1, szBuffer2, szBuffer3, szBuffer4, szBuffer5, szBuffer6, szBuffer7, szBuffer8);
*/

// CSV floats

    float temps[64];

    for (uint8_t i = 0; i < 64; i++)
        temps[i] = GridEYE::getPixelTemperature(i);

    char szBuffer1[60] = {'\0'};
    snprintf(szBuffer1, sizeof(szBuffer1), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x00], temps[0x01], temps[0x02], temps[0x03], temps[0x04], temps[0x05], temps[0x06], temps[0x07]);
    char szBuffer2[60] = {'\0'};
    snprintf(szBuffer2, sizeof(szBuffer2), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x08], temps[0x09], temps[0x0A], temps[0x0B], temps[0x0C], temps[0x0D], temps[0x0E], temps[0x0F]);
    char szBuffer3[60] = {'\0'};
    snprintf(szBuffer3, sizeof(szBuffer3), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x10], temps[0x11], temps[0x12], temps[0x13], temps[0x14], temps[0x15], temps[0x16], temps[0x17]);
    char szBuffer4[60] = {'\0'};
    snprintf(szBuffer4, sizeof(szBuffer4), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x18], temps[0x19], temps[0x1A], temps[0x1B], temps[0x1C], temps[0x1D], temps[0x1E], temps[0x1F]);
    char szBuffer5[60] = {'\0'};
    snprintf(szBuffer5, sizeof(szBuffer5), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x20], temps[0x21], temps[0x22], temps[0x23], temps[0x24], temps[0x25], temps[0x26], temps[0x27]);
    char szBuffer6[60] = {'\0'};
    snprintf(szBuffer6, sizeof(szBuffer6), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x28], temps[0x29], temps[0x2A], temps[0x2B], temps[0x2C], temps[0x2D], temps[0x2E], temps[0x2F]);
    char szBuffer7[60] = {'\0'};
    snprintf(szBuffer7, sizeof(szBuffer7), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x30], temps[0x31], temps[0x32], temps[0x33], temps[0x34], temps[0x35], temps[0x36], temps[0x37]);
    char szBuffer8[60] = {'\0'};
    snprintf(szBuffer8, sizeof(szBuffer8), "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", temps[0x38], temps[0x39], temps[0x3A], temps[0x3B], temps[0x3C], temps[0x3D], temps[0x3E], temps[0x3F]);

    char szBuffer[490] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%s,%s,%s,%s,%s,%s,%s,%s", szBuffer1, szBuffer2, szBuffer3, szBuffer4, szBuffer5, szBuffer6, szBuffer7, szBuffer8);

    std::string theString = szBuffer;

    return theString;
}


