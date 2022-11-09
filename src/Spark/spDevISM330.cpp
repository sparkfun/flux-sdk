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

#include "spDevISM330.h"

#define kISM330AddressDefault 0x6B
#define kISM330AddressAlt 0x6A

// Define our class static variables - allocs storage for them

uint8_t spDevISM330::defaultDeviceAddress[] = {kISM330AddressDefault, kISM330AddressAlt, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevISM330);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevISM330::spDevISM330()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ISM330 Inertial Measurement Unit");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    spRegister(accelX, "Accel X (milli-g)", "Accelerometer X (milli-g)");
    spRegister(accelY, "Accel Y (milli-g)", "Accelerometer Y (milli-g)");
    spRegister(accelZ, "Accel Z (milli-g)", "Accelerometer Z (milli-g)");
    spRegister(gyroX, "Gyro X (milli-dps)", "Gyro X (milli-dps)");
    spRegister(gyroY, "Gyro Y (milli-dps)", "Gyro Y (milli-dps)");
    spRegister(gyroZ, "Gyro Z (milli-dps)", "Gyro Z (milli-dps)");
    spRegister(temperature, "Temperature (C)", "The gyro temperature in degrees C");

    // Register properties
    spRegister(accelDataRate, "Accel Data Rate (Hz)", "Accelerometer Data Rate (Hz)");
    spRegister(accelFullScale, "Accel Full Scale (g)", "Accelerometer Full Scale (g)");
    spRegister(gyroDataRate, "Gyro Data Rate (Hz)", "Gyro Data Rate (Hz)");
    spRegister(gyroFullScale, "Gyro Full Scale (dps)", "Gyro Full SCale (dps)");
    spRegister(accelFilterLP2, "Accel Filter LP2", "Accelerometer Filter LP2");
    spRegister(gyroFilterLP1, "Gyro Filter LP1", "Gyro Filter LP1");
    spRegister(accelSlopeFilter, "Accel Slope Filter", "Accelerometer Slope Filter");
    spRegister(gyroLP1Bandwidth, "Gyro LP1 Filter Bandwidth", "Gyro LP1 Filter Bandwidth");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevISM330::isConnected(spBusI2C &i2cDriver, uint8_t address)
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
bool spDevISM330::onInitialize(TwoWire &wirePort)
{

    bool result = SparkFun_ISM330DHCX::begin(wirePort, address());
    if (result)
    {
        SparkFun_ISM330DHCX::deviceReset();
        unsigned long startTime = millis();
        bool reset = false;
        while (!reset && (millis() < (startTime + 2000))) // Time out after 2 seconds
        {
            reset = SparkFun_ISM330DHCX::getDeviceReset();
            delay(1);
        }
        if (reset)
        {
            delay(100);
            result &= SparkFun_ISM330DHCX::setDeviceConfig();
            result &= SparkFun_ISM330DHCX::setBlockDataUpdate();
            result &= SparkFun_ISM330DHCX::setAccelDataRate(_accel_data_rate);
            result &= SparkFun_ISM330DHCX::setAccelFullScale(_accel_full_scale);
            result &= SparkFun_ISM330DHCX::setGyroDataRate(_gyro_data_rate);
            result &= SparkFun_ISM330DHCX::setGyroFullScale(_gyro_full_scale);
            result &= SparkFun_ISM330DHCX::setAccelFilterLP2(_accel_filter_lp2);
            result &= SparkFun_ISM330DHCX::setAccelSlopeFilter(_accel_slope_filter);
            result &= SparkFun_ISM330DHCX::setGyroFilterLP1(_gyro_filter_lp1);
            result &= SparkFun_ISM330DHCX::setGyroLP1Bandwidth(_gyro_lp1_bandwidth);
            if (!result)
                spLog_E("ISM330 onInitialize: device configuration failed");
        }
        else
        {
            spLog_E("ISM330 onInitialize: device did not reset");
            result = false;
        }
    }
    else
        spLog_E("ISM330 onInitialize: device did not begin");
    _begun = result;
    return result;
}

// GETTER methods for output params
float spDevISM330::read_accel_x()
{
    if (!_accelX)
    {
        if (SparkFun_ISM330DHCX::getAccel(&_accelData))
        {
            _accelY = true;
            _accelZ = true;
        }
    }
    _accelX = false;
    return _accelData.xData;
}
float spDevISM330::read_accel_y()
{
    if (!_accelY)
    {
        if (SparkFun_ISM330DHCX::getAccel(&_accelData))
        {
            _accelX = true;
            _accelZ = true;
        }
    }
    _accelY = false;
    return _accelData.yData;
}
float spDevISM330::read_accel_z()
{
    if (!_accelZ)
    {
        if (SparkFun_ISM330DHCX::getAccel(&_accelData))
        {
            _accelX = true;
            _accelY = true;
        }
    }
    _accelZ = false;
    return _accelData.zData;
}
float spDevISM330::read_gyro_x()
{
    if (!_gyroX)
    {
        if (SparkFun_ISM330DHCX::getGyro(&_gyroData))
        {
            _gyroY = true;
            _gyroZ = true;
        }
    }
    _gyroX = false;
    return _gyroData.xData;
}
float spDevISM330::read_gyro_y()
{
    if (!_gyroY)
    {
        if (SparkFun_ISM330DHCX::getGyro(&_gyroData))
        {
            _gyroX = true;
            _gyroZ = true;
        }
    }
    _gyroY = false;
    return _gyroData.yData;
}
float spDevISM330::read_gyro_z()
{
    if (!_gyroZ)
    {
        if (SparkFun_ISM330DHCX::getGyro(&_gyroData))
        {
            _gyroX = true;
            _gyroY = true;
        }
    }
    _gyroZ = false;
    return _gyroData.zData;
}
float spDevISM330::read_temperature()
{
    float temp = (float)SparkFun_ISM330DHCX::getTemp();
    temp /= 256; // Temperature sensitivity 256 LSB/°C
    temp += 25; // The output of the temperature sensor is 0 LSB (typ.) at 25 °C
    return temp;
}


uint8_t spDevISM330::get_accel_data_rate()
{
    return _accel_data_rate;
}
void spDevISM330::set_accel_data_rate(uint8_t rate)
{
    _accel_data_rate = rate;
    if (_begun)
        SparkFun_ISM330DHCX::setAccelDataRate(rate);
}
uint8_t spDevISM330::get_accel_full_scale()
{
    return _accel_full_scale;
}
void spDevISM330::set_accel_full_scale(uint8_t scale)
{
    _accel_full_scale = scale;
    if (_begun)
        SparkFun_ISM330DHCX::setAccelFullScale(scale);
}
uint8_t spDevISM330::get_gyro_data_rate()
{
    return _gyro_data_rate;
}
void spDevISM330::set_gyro_data_rate(uint8_t rate)
{
    _gyro_data_rate = rate;
    if (_begun)
        SparkFun_ISM330DHCX::setGyroDataRate(rate);
}
uint8_t spDevISM330::get_gyro_full_scale()
{
    return _gyro_full_scale;
}
void spDevISM330::set_gyro_full_scale(uint8_t scale)
{
    _gyro_full_scale = scale;
    if (_begun)
        SparkFun_ISM330DHCX::setGyroFullScale(scale);
}
uint8_t spDevISM330::get_accel_filter_lp2()
{
    return (uint8_t)_accel_filter_lp2;
}
void spDevISM330::set_accel_filter_lp2(uint8_t enable)
{
    _accel_filter_lp2 = (bool)enable;
    if (_begun)
        SparkFun_ISM330DHCX::setAccelFilterLP2((bool)enable);
}
uint8_t spDevISM330::get_gyro_filter_lp1()
{
    return (uint8_t)_gyro_filter_lp1;
}
void spDevISM330::set_gyro_filter_lp1(uint8_t enable)
{
    _gyro_filter_lp1 = (bool)enable;
    if (_begun)
        SparkFun_ISM330DHCX::setGyroFilterLP1((bool)enable);
}
uint8_t spDevISM330::get_accel_slope_filter()
{
    return _accel_slope_filter;
}
void spDevISM330::set_accel_slope_filter(uint8_t slope)
{
    _accel_slope_filter = slope;
    if (_begun)
        SparkFun_ISM330DHCX::setAccelSlopeFilter(slope);
}
uint8_t spDevISM330::get_gyro_lp1_bandwidth()
{
    return _gyro_lp1_bandwidth;
}
void spDevISM330::set_gyro_lp1_bandwidth(uint8_t bw)
{
    _gyro_lp1_bandwidth = bw;
    if (_begun)
        SparkFun_ISM330DHCX::setGyroLP1Bandwidth(bw);
}
