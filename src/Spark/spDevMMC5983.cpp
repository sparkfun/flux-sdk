/*
 *
 * QwiicDevMMC5983.cpp
 *
 *  Device object for the MMC5983 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevMMC5983.h"

#define kMMC5983AddressDefault 0x30

// Define our class static variables - allocs storage for them

uint8_t spDevMMC5983::defaultDeviceAddress[] = {kMMC5983AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevMMC5983);

//----------------------------------------------------------------------------------------------------------
// Base class implementation
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevMMC5983Base::spDevMMC5983Base() : _begun{false}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(kMMC5983DeviceName, "MMC5983 Magnetometer");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    spRegister(magX, "X Field (Gauss)", "The X field strength in Gauss");
    spRegister(magY, "Y Field (Gauss)", "The Y field strength in Gauss");
    spRegister(magZ, "Z Field (Gauss)", "The Z field strength in Gauss");
    magX.setPrecision(5);
    magY.setPrecision(5);
    magZ.setPrecision(5);
    spRegister(temperature, "Temperature (C)", "The ambient temperature in degrees C");

    // Register properties
    spRegister(filterBandwidth, "Filter Bandwidth (Hz)", "The filter bandwidth in Hz");
    spRegister(autoReset, "Auto-Reset", "Auto-Reset");
}

bool spDevMMC5983Base::onInitialize(void)
{
    bool result = true;
    result &= SFE_MMC5983MA::softReset();
    result &= SFE_MMC5983MA::setFilterBandwidth(_filter_bandwidth);
    if (_auto_reset)
        result &= SFE_MMC5983MA::enableAutomaticSetReset();
    else
        result &= SFE_MMC5983MA::disableAutomaticSetReset();

    _begun = result;

    return result;
}

// GETTER methods for output params
double spDevMMC5983Base::read_x()
{
    if (!_magX)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magY = true;
        _magZ = true;
    }
    _magX = false;
    return ((double)_rawX - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
double spDevMMC5983Base::read_y()
{
    if (!_magY)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magX = true;
        _magZ = true;
    }
    _magY = false;
    return ((double)_rawY - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
double spDevMMC5983Base::read_z()
{
    if (!_magZ)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magX = true;
        _magY = true;
    }
    _magZ = false;
    return ((double)_rawZ - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
int spDevMMC5983Base::read_temperature()
{
    return SFE_MMC5983MA::getTemperature();
}

uint16_t spDevMMC5983Base::get_filter_bandwidth()
{
    if (_begun)
        return SFE_MMC5983MA::getFilterBandwith();
    else
        return _filter_bandwidth;
}
void spDevMMC5983Base::set_filter_bandwidth(uint16_t bw)
{
    _filter_bandwidth = bw;
    if (_begun)
        SFE_MMC5983MA::setFilterBandwidth(bw);
}
uint8_t spDevMMC5983Base::get_auto_reset()
{
    if (_begun)
        return (SFE_MMC5983MA::isAutomaticSetResetEnabled() ? 1 : 0);
    else
        return (uint8_t)_auto_reset;
}
void spDevMMC5983Base::set_auto_reset(uint8_t enable)
{
    _auto_reset = (bool)enable;
    if (_begun)
    {
        if (enable)
            SFE_MMC5983MA::enableAutomaticSetReset();
        else
            SFE_MMC5983MA::disableAutomaticSetReset();
    }
}

//----------------------------------------------------------------------------------------------------------
// I2C Implementation
//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevMMC5983::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t devID = 0;
    bool couldBe5983 = i2cDriver.readRegister(address, PROD_ID_REG, &devID);
    couldBe5983 &= devID == PROD_ID;
    return couldBe5983;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMMC5983::onInitialize(TwoWire &wirePort)
{

    bool result = SFE_MMC5983MA::begin(wirePort);
    if (result)
        result = spDevMMC5983Base::onInitialize();
    else
        flxLog_E("MMC5983 onInitialize: device did not begin");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// SPI Implementation
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMMC5983_SPI::onInitialize(SPIClass &spiPort)
{

    bool result = SFE_MMC5983MA::begin(chipSelect(), spiPort);
    if (result)
        result = spDevMMC5983Base::onInitialize();
    else
        flxLog_E("MMC5983_SPI onInitialize: device did not begin");

    return result;
}