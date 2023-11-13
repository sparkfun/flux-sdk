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
 *  flxDevBMI270.cpp
 *
 *  Spark Device object for the BMI270 6DoF IMU device.
 *
 *
 *
 */

#include <Arduino.h>

#include "flxDevBMI270.h"

#define kBMI270AddressDefault BMI2_I2C_PRIM_ADDR  // 0x68
#define kBMI270AddressSecondary BMI2_I2C_SEC_ADDR // 0x69

#define kDefaultProdID 0x24

// Define our class static variables - allocs storage for them
uint8_t flxDevBMI270::defaultDeviceAddress[] = {kBMI270AddressDefault, kBMI270AddressSecondary,
                                                kSparkDeviceAddressNull};

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevBMI270);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevBMI270::flxDevBMI270() : _validData{false}
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "BMI270 6DoF IMU");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties and parameters

    // Register Properties
    flxRegister(accelDataRate, "Accel ODR", "Output Data Rate in Hz");
    flxRegister(accelPowerMode, "Accel  Power", "Accelerometer Power mode");
    flxRegister(accelFilterBW, "Accel Filter Bandwidth", "Filter BW for the accelerometer");
    flxRegister(accelRange, "Accel Data Range", "Range for the accelerometer in g");
    flxRegister(gyroDataRate, "Gyros ODR", "Output Data Rate in Hz");
    flxRegister(gyroFilterPowerMode, "Gyro Filter Power Mode", "Power Mode for the Filter");
    flxRegister(gyroNoisePowerMode, "Gyro Noise Power Mode", "Low noise mode for precision yaw rate sensing");
    flxRegister(gyroFilterBW, "Gyro Filter Bandwidth", "Filter BW for the gyroscope");
    flxRegister(gyroRange, "Gyro Data Range", "Scale range in deg/sec");

    // Register Output Parameters
    flxRegister(accelX, "Accel X (g)", "Accelerometer X");
    flxRegister(accelY, "Accel Y (g)", "Accelerometer Y");
    flxRegister(accelZ, "Accel Z (g)", "Accelerometer Z");
    flxRegister(gyroX, "Gyro X (deg/s)", "Gyroscope X");
    flxRegister(gyroY, "Gyro Y (deg/s)", "Gyroscope Y");
    flxRegister(gyroZ, "Gyro Z (deg/s)", "Gyroscope Z");
    flxRegister(temperature, "Temp (C)", "Temperature (C)");
    flxRegister(stepCount, "Steps", "# of steps");

    // Register Input Parameters
    flxRegister(resetStepCount, "Reset Step Count", "Resets the number of steps");
}

//-----------------------------------------------------------------------------
// Static method used to determine if device is connected before creating the
// object (if creating dynamically)
bool flxDevBMI270::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t readValue;

    if (!i2cDriver.readRegister(address, BMI2_CHIP_ID_ADDR, &readValue))
        return false;

    return (readValue == kDefaultProdID);
}

//-----------------------------------------------------------------------------
// onInitialize
//
// Called during the startup/initialization of the driver (after the
// constructor is called).
//
// Place to initialize the underlying device library/driver.
bool flxDevBMI270::onInitialize(TwoWire &wirePort)
{
    uint8_t retries = 3;

    while (BMI2_OK != beginI2C(address(), wirePort))
    {
        delay(200);
        retries--;

        if (retries <= 0)
        {
            flxLog_E(F("%s: Sensor failed to respond"), name());
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
///
/// @brief Called right before data parameters are read -
///

bool flxDevBMI270::execute(void)
{
    _validData = getSensorData() == BMI2_OK;

    if (!_validData)
        flxLog_E(F("%s: Error reading sensor data values"), name());

    return _validData;
}
/* Read methods for parameters */

float flxDevBMI270::read_accel_x()
{
    return _validData ? data.accelX : 0.;
}

float flxDevBMI270::read_accel_y()
{
    return _validData ? data.accelY : 0.;
}

float flxDevBMI270::read_accel_z()
{
    return _validData ? data.accelZ : 0.;
}

float flxDevBMI270::read_gyro_x()
{
    return _validData ? data.gyroX : 0.;
}

float flxDevBMI270::read_gyro_y()
{
    return _validData ? data.gyroY : 0.;
}

float flxDevBMI270::read_gyro_z()
{
    return _validData ? data.gyroZ : 0.;
}

float flxDevBMI270::read_temperature()
{
    if (BMI2_OK != BMI270::getTemperature(&_tempC))
        flxLog_E(F("%s: Failed to read temperature data"), name());

    return _tempC;
}

uint flxDevBMI270::read_step_count()
{
    if (!_stepCounterEnabled)
    {
        if (!enable_step_counter(true))
            flxLog_E(F("%s: Failed to enable step counting feature"), name());
    }

    if (_stepCounterEnabled)
    {
        if (BMI2_OK != BMI270::getStepCount(&_countedSteps))
            flxLog_E(F("%s: Failed to get step count. Logging last good value"), name());
        ;
    }

    return _countedSteps;
}

/* Write methods for parameters */

void flxDevBMI270::write_reset_step_count()
{
    if (_stepCounterEnabled)
    {
        if (BMI2_OK != BMI270::resetStepCount())
            flxLog_E(F("%s: Failed to reset step count"), name());
    }
}

/* Getter methods for properties */

uint8_t flxDevBMI270::get_accel_odr()
{
    return accelConfig.cfg.acc.odr;
}

uint8_t flxDevBMI270::get_accel_power_mode()
{
    return accelConfig.cfg.acc.filter_perf;
}

uint8_t flxDevBMI270::get_accel_filter_bandwidth()
{
    return accelConfig.cfg.acc.bwp;
}

uint8_t flxDevBMI270::get_accel_range()
{
    return accelConfig.cfg.acc.range;
}

uint8_t flxDevBMI270::get_gyro_odr()
{
    return gyroConfig.cfg.gyr.odr;
}

uint8_t flxDevBMI270::get_gyro_power_mode_filter()
{
    return gyroConfig.cfg.gyr.filter_perf;
}

uint8_t flxDevBMI270::get_gyro_power_mode_noise()
{
    return gyroConfig.cfg.gyr.noise_perf;
}

uint8_t flxDevBMI270::get_gyro_filter_bandwidth()
{
    return gyroConfig.cfg.gyr.bwp;
}

uint8_t flxDevBMI270::get_gyro_range()
{
    return gyroConfig.cfg.gyr.range;
}

/* Setter methods for properties */

void flxDevBMI270::set_accel_odr(uint8_t dataRate)
{
    if (BMI2_OK != setAccelODR(dataRate))
        flxLog_E(F("%s: Could not set ODR"), name());
    else
        accelConfig.cfg.acc.odr = dataRate;
}

void flxDevBMI270::set_accel_power_mode(uint8_t powerMode)
{
    if (BMI2_OK != setAccelPowerMode(powerMode))
        flxLog_E(F("%s: Could not set mode"), name());
    else
        accelConfig.cfg.acc.filter_perf = powerMode;
}

void flxDevBMI270::set_accel_filter_bandwidth(uint8_t bandwidth)
{
    if (BMI2_OK != setAccelFilterBandwidth(bandwidth))
        flxLog_E(F("%s: Could not set bandwidth"), name());
    else
        accelConfig.cfg.acc.bwp = bandwidth;
}

void flxDevBMI270::set_accel_range(uint8_t range)
{
    uint8_t prevRange = accelConfig.cfg.acc.range;

    accelConfig.cfg.acc.range = range;

    if (BMI2_OK != setConfig(accelConfig))
    {
        flxLog_E(F("%s: Could not set range"), name());
        accelConfig.cfg.acc.range = prevRange;
    }
}

void flxDevBMI270::set_gyro_odr(uint8_t dataRate)
{
    if (BMI2_OK != setGyroODR(dataRate))
        flxLog_E(F("%s: Could not set ODR"), name());
    else
        gyroConfig.cfg.gyr.odr = dataRate;
}

void flxDevBMI270::set_gyro_power_mode_filter(uint8_t powerMode)
{
    if (BMI2_OK != setGyroPowerMode(powerMode, gyroConfig.cfg.gyr.noise_perf))
        flxLog_E(F("%s: Could not set gyro filter mode"), name());
    else
        gyroConfig.cfg.gyr.filter_perf = powerMode;
}

void flxDevBMI270::set_gyro_power_mode_noise(uint8_t powerMode)
{
    if (BMI2_OK != setGyroPowerMode(gyroConfig.cfg.gyr.filter_perf, powerMode))
        flxLog_E(F("%s: Could not set gyro power mode."), name());
    else
        gyroConfig.cfg.gyr.noise_perf = powerMode;
}

void flxDevBMI270::set_gyro_filter_bandwidth(uint8_t bandwidth)
{
    if (BMI2_OK != setGyroFilterBandwidth(bandwidth))
        flxLog_E(F("%s: Could not set bandwidth"), name());
    else
        gyroConfig.cfg.gyr.bwp = bandwidth;
}

void flxDevBMI270::set_gyro_range(uint8_t range)
{
    uint8_t prevRange = gyroConfig.cfg.gyr.range;

    gyroConfig.cfg.gyr.range = range;

    if (BMI2_OK != setConfig(gyroConfig))
    {
        flxLog_E(F("%s: Could not set range"), name());
        gyroConfig.cfg.gyr.range = prevRange;
    }
}

bool flxDevBMI270::enable_step_counter(bool enabled)
{
    if (enabled && !_stepCounterEnabled)
    {
        if (BMI2_OK != enableFeature(BMI2_STEP_COUNTER))
            flxLog_E(F("%s: Could not enable step counter"), name());

        _stepCounterEnabled = true;
    }
    else if (!enabled && _stepCounterEnabled)
    {
        if (BMI2_OK != disableFeature(BMI2_STEP_COUNTER))
            flxLog_E(F("%s: Could not disable step counter"), name());

        _stepCounterEnabled = false;
    }
}
