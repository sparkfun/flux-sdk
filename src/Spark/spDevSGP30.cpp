/*
 *
 *  spDevSGP30.h
 *
 *  Spark Device object for the SGP30 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevSGP30.h"

// Define our class static variables - allocs storage for them

#define kSGP30AddressDefault 0x58 // _SGP30Address is private...

uint8_t spDevSGP30::defaultDeviceAddress[] = {kSGP30AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevSGP30);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevSGP30::spDevSGP30()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SGP30 Air Quality Sensor");

    // Register parameters
    spRegister(tvoc, "TVOC (PPB)", "Total Volatile Organic Compounds in Parts Per Billion");
    spRegister(co2, "CO2 (PPM)", "CO2 concentration in Parts Per Million");
    spRegister(h2, "H2 (PPM)", "Hydrogen concentration in Parts Per Million");
    spRegister(ethanol, "Ethanol (PPM)", "Ethanol concentration in Parts Per Million");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevSGP30::isConnected(spBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    if (!i2cDriver.write(address, (uint8_t *)get_serial_id, 2))
        return false;
    delay(1);

    uint8_t serialNo[9]; // 3 * (Two bytes plus CRC)
    if (i2cDriver.receiveResponse(address, serialNo, 9) != 9)
        return false;

    // Check only the 3rd CRC (there are three in total)
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 6; x < 8; x++)
    {
        crc ^= serialNo[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31); // x^8+x^5+x^4+1 = 0x31
            else
                crc <<= 1;
        }
    }

    return (crc == serialNo[8]);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevSGP30::onInitialize(TwoWire &wirePort)
{

    bool result = SGP30::begin(wirePort);
    SGP30::initAirQuality();
    return result;
}

// GETTER methods for output params
uint spDevSGP30::read_tvoc()
{
      if (_tvoc == false)
      {
        SGP30::measureAirQuality();
        SGP30::measureRawSignals();
        _co2 = true;
        _h2 = true;
        _ethanol = true;
      }
      _tvoc = false;
    return SGP30::TVOC;
}

uint spDevSGP30::read_co2()
{
      if (_co2 == false)
      {
        SGP30::measureAirQuality();
        SGP30::measureRawSignals();
        _tvoc = true;
        _h2 = true;
        _ethanol = true;
      }
      _co2 = false;
    return SGP30::CO2;
}

uint spDevSGP30::read_h2()
{
      if (_h2 == false)
      {
        SGP30::measureAirQuality();
        SGP30::measureRawSignals();
        _tvoc = true;
        _co2 = true;
        _ethanol = true;
      }
      _h2 = false;
    return SGP30::H2;
}

uint spDevSGP30::read_ethanol()
{
      if (_ethanol == false)
      {
        SGP30::measureAirQuality();
        SGP30::measureRawSignals();
        _tvoc = true;
        _co2 = true;
        _h2 = true;
      }
      _ethanol = false;
    return SGP30::ethanol;
}

//----------------------------------------------------------------------------------------------------------
// RW Properties

// TO DO: add baselineCO2 and baseline TVOC as RW properties