/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
/*
 *
 * QwiicDevISM330.cpp
 *
 *  Device object for the ISM330 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevISM330.h"

#define kISM330AddressDefault 0x6B
#define kISM330AddressAlt 0x6A

// Define our class static variables - allocs storage for them

uint8_t flxDevISM330::defaultDeviceAddress[] = {kISM330AddressDefault, kISM330AddressAlt, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register the I2C version class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevISM330);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevISM330Base::flxDevISM330Base()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(kISM330DeviceName, "ISM330 Inertial Measurement Unit");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    flxRegister(accelX, "Accel X (milli-g)", "Accelerometer X (milli-g)");
    flxRegister(accelY, "Accel Y (milli-g)", "Accelerometer Y (milli-g)");
    flxRegister(accelZ, "Accel Z (milli-g)", "Accelerometer Z (milli-g)");
    flxRegister(gyroX, "Gyro X (milli-dps)", "Gyro X (milli-dps)");
    flxRegister(gyroY, "Gyro Y (milli-dps)", "Gyro Y (milli-dps)");
    flxRegister(gyroZ, "Gyro Z (milli-dps)", "Gyro Z (milli-dps)");
    flxRegister(temperature, "Temperature (C)", "The ambient temperature in degrees C");

    // Register properties
    flxRegister(accelDataRate, "Accel Data Rate (Hz)", "Accelerometer Data Rate (Hz)");
    flxRegister(accelFullScale, "Accel Full Scale (g)", "Accelerometer Full Scale (g)");
    flxRegister(gyroDataRate, "Gyro Data Rate (Hz)", "Gyro Data Rate (Hz)");
    flxRegister(gyroFullScale, "Gyro Full Scale (dps)", "Gyro Full Scale (dps)");
    flxRegister(accelFilterLP2, "Accel Filter LP2", "Accelerometer Filter LP2");
    flxRegister(gyroFilterLP1, "Gyro Filter LP1", "Gyro Filter LP1");
    flxRegister(accelSlopeFilter, "Accel Slope Filter", "Accelerometer Slope Filter");
    flxRegister(gyroLP1Bandwidth, "Gyro LP1 Filter Bandwidth", "Gyro LP1 Filter Bandwidth");
}

// Base version of on Initialize

bool flxDevISM330Base::onInitialize(void)
{

    bool result = true;

    deviceReset();
    unsigned long startTime = millis();
    bool reset = false;

    while (!reset && (millis() < (startTime + 2000))) // Time out after 2 seconds
    {
        reset = getDeviceReset();
        delay(1);
    }
    if (reset)
    {
        delay(100);
        result &= setDeviceConfig();
        result &= setBlockDataUpdate();
        result &= setAccelDataRate(_accel_data_rate);
        result &= setAccelFullScale(_accel_full_scale);
        result &= setGyroDataRate(_gyro_data_rate);
        result &= setGyroFullScale(_gyro_full_scale);
        result &= setAccelFilterLP2(_accel_filter_lp2);
        result &= setAccelSlopeFilter(_accel_slope_filter);
        result &= setGyroFilterLP1(_gyro_filter_lp1);
        result &= setGyroLP1Bandwidth(_gyro_lp1_bandwidth);
        if (!result)
            flxLog_E("ISM330 onInitialize: device configuration failed");
    }
    else
    {
        flxLog_E("ISM330 onInitialize: device did not reset");
        result = false;
    }
    return result;
}

// GETTER methods for output params
float flxDevISM330Base::read_accel_x()
{
    if (!_accelX)
    {
        if (getAccel(&_accelData))
        {
            _accelY = true;
            _accelZ = true;
        }
    }
    _accelX = false;
    return _accelData.xData;
}
float flxDevISM330Base::read_accel_y()
{
    if (!_accelY)
    {
        if (getAccel(&_accelData))
        {
            _accelX = true;
            _accelZ = true;
        }
    }
    _accelY = false;
    return _accelData.yData;
}
float flxDevISM330Base::read_accel_z()
{
    if (!_accelZ)
    {
        if (getAccel(&_accelData))
        {
            _accelX = true;
            _accelY = true;
        }
    }
    _accelZ = false;
    return _accelData.zData;
}
float flxDevISM330Base::read_gyro_x()
{
    if (!_gyroX)
    {
        if (getGyro(&_gyroData))
        {
            _gyroY = true;
            _gyroZ = true;
        }
    }
    _gyroX = false;
    return _gyroData.xData;
}
float flxDevISM330Base::read_gyro_y()
{
    if (!_gyroY)
    {
        if (getGyro(&_gyroData))
        {
            _gyroX = true;
            _gyroZ = true;
        }
    }
    _gyroY = false;
    return _gyroData.yData;
}
float flxDevISM330Base::read_gyro_z()
{
    if (!_gyroZ)
    {
        if (getGyro(&_gyroData))
        {
            _gyroX = true;
            _gyroY = true;
        }
    }
    _gyroZ = false;
    return _gyroData.zData;
}
float flxDevISM330Base::read_temperature()
{
    float temp = (float)getTemp();
    temp /= 256; // Temperature sensitivity 256 LSB/°C
    temp += 25;  // The output of the temperature sensor is 0 LSB (typ.) at 25 °C
    return temp;
}

uint8_t flxDevISM330Base::get_accel_data_rate()
{
    return _accel_data_rate;
}
void flxDevISM330Base::set_accel_data_rate(uint8_t rate)
{
    _accel_data_rate = rate;
    if (isInitialized())
        setAccelDataRate(rate);
}
uint8_t flxDevISM330Base::get_accel_full_scale()
{
    return _accel_full_scale;
}
void flxDevISM330Base::set_accel_full_scale(uint8_t scale)
{
    _accel_full_scale = scale;
    if (isInitialized())
        setAccelFullScale(scale);
}
uint8_t flxDevISM330Base::get_gyro_data_rate()
{
    return _gyro_data_rate;
}
void flxDevISM330Base::set_gyro_data_rate(uint8_t rate)
{
    _gyro_data_rate = rate;
    if (isInitialized())
        setGyroDataRate(rate);
}
uint8_t flxDevISM330Base::get_gyro_full_scale()
{
    return _gyro_full_scale;
}
void flxDevISM330Base::set_gyro_full_scale(uint8_t scale)
{
    _gyro_full_scale = scale;
    if (isInitialized())
        setGyroFullScale(scale);
}
uint8_t flxDevISM330Base::get_accel_filter_lp2()
{
    return (uint8_t)_accel_filter_lp2;
}
void flxDevISM330Base::set_accel_filter_lp2(uint8_t enable)
{
    _accel_filter_lp2 = (bool)enable;
    if (isInitialized())
        setAccelFilterLP2((bool)enable);
}
uint8_t flxDevISM330Base::get_gyro_filter_lp1()
{
    return (uint8_t)_gyro_filter_lp1;
}
void flxDevISM330Base::set_gyro_filter_lp1(uint8_t enable)
{
    _gyro_filter_lp1 = (bool)enable;
    if (isInitialized())
        setGyroFilterLP1((bool)enable);
}
uint8_t flxDevISM330Base::get_accel_slope_filter()
{
    return _accel_slope_filter;
}
void flxDevISM330Base::set_accel_slope_filter(uint8_t slope)
{
    _accel_slope_filter = slope;
    if (isInitialized())
        setAccelSlopeFilter(slope);
}
uint8_t flxDevISM330Base::get_gyro_lp1_bandwidth()
{
    return _gyro_lp1_bandwidth;
}
void flxDevISM330Base::set_gyro_lp1_bandwidth(uint8_t bw)
{
    _gyro_lp1_bandwidth = bw;
    if (isInitialized())
        setGyroLP1Bandwidth(bw);
}

//----------------------------------------------------------------------------------------------------------
// I2C Version of the driver
//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevISM330::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t devID = 0;
    bool couldBe330 = i2cDriver.readRegister(address, ISM330DHCX_WHO_AM_I, &devID);
    couldBe330 &= devID == ISM330DHCX_ID;
    return couldBe330;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevISM330::onInitialize(TwoWire &wirePort)
{
    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_i2cBus, address());

    // Give the I2C port provided by the user to the I2C bus class.
    _i2cBus.init(wirePort, true);

    // Initialize the system -
    bool result = this->QwDevISM330DHCX::init();

    if (result)
        result = flxDevISM330Base::onInitialize();
    else
        flxLog_E("ISM330 onInitialize: device did not begin");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// SPI Version of the driver
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevISM330_SPI::onInitialize(SPIClass &spiPort)
{

    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_spiBus);

    SPISettings spiSettings = SPISettings(3000000, MSBFIRST, SPI_MODE3);

    _spiBus.init(spiPort, spiSettings, chipSelect(), true);

    bool result = this->QwDevISM330DHCX::init();
    if (result)
        result = flxDevISM330Base::onInitialize();
    else
        flxLog_E("ISM330 SPI onInitialize: device did not begin");

    return result;
}