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

#define kBMI270AddressDefault BMI2_I2C_PRIM_ADDR // 0x68
#define kBMI270AddressSecondary BMI2_I2C_SEC_ADDR // 0x69

#define kDefaultProdID 0x24

// Define our class static variables - allocs storage for them
uint8_t flxDevBMI270::defaultDeviceAddress[] = {kBMI270AddressDefault, kBMI270AddressSecondary, kSparkDeviceAddressNull};

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevBMI270);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevBMI270::flxDevBMI270()
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "BMI270 6DoF IMU");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties and parameters

    // Register Properties
    flxRegister(accelDataRate, "Accelerometer ODR", "Output Data Rate for the accelerometer in Hz.");
    flxRegister(accelPowerMode, "Accelerometer Power Mode", "Power mode for the accelerometer.");
    flxRegister(accelFilterBW, "Accelerometer Filter Bandwidth", "Filter BW for the accelerometer in order of Performance or Power Saving Mode.");
    flxRegister(accelRange, "Accelerometer Data Range", "Scale range for the accelerometer in g.")
    flxRegister(gyroDataRate, "Gyroscope ODR", "Output Data Rate for the Gyroscope in Hz.");
    flxRegister(gyroFilterPowerMode, "Gyroscope Filter Power Mode", "Power Mode for the Gyroscope Filter.");
    flxRegister(gyroNoisePowerMode, "Gyroscope Noise Power Mode", "Low noise mode for precision yaw rate sensing.");
    flxRegister(gyroFilterBW, "Gyroscope Filter Bandwidth", "Filter BW for the gyroscope.");
    flxRegister(gyroRange, "Gyroscope Data Range", "Scale range for the gyroscope in deg/s.");

    // Register Output Parameters
    flxRegister(accelX, "Accel X (g)", "Accelerometer X (g)");
    flxRegister(accelY, "Accel Y (g)", "Accelerometer Y (g)");
    flxRegister(accelZ, "Accel Z (g)", "Accelerometer Z (g)");
    flxRegister(gyroX, "Gyro X (deg/s)", "Gyroscope X (deg/s)");
    flxRegister(gyroY, "Gyro Y (deg/s)", "Gyroscope Y (deg/s)");
    flxRegister(gyroZ, "Gyro Z (deg/s)", "Gyroscope Z (deg/s)");
    flxRegister(temperature, "Temp (C)", "Temperature (C)");
    flxRegister(stepCount, "Steps", "# of steps");

    // Register Input Parameters
    flxRegister(resetStepCount, "Reset Step Count", "Resets the number of steps counted.");
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

    if(!i2cDriver.readRegister(address, BMI2_CHIP_ID_ADDR, &readValue)) {
        flxLog_E("BMI270::isConnected: Failed to read prodID.");
        return false;
    }

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

    while (BMI2_OK != beginI2C(address(), wirePort)) {
        delay(200);
        retries--;
        flxLog_D("BMI270::onInitialize: Begin #%d failed, retrying...", (3-retries));
        if (retries <= 0) {
            flxLog_E("BMI270::onInitialize: Sensor failed to respond!");
            return false;
        }
    }

    _sensorInitialized = true;

    // TODO: Finish Initialization

    return true;
}

/* Read methods for parameters */

float flxDevBMI270::read_accel_x()
{
    if (!_accelX) {
        if (BMI2_OK == getSensorData()) {
            _accelY = true;
            _accelZ = true;
            _gyroX = true;
            _gyroY = true;
            _gyroZ = true;
        }
        else {
            flxLog_E("BMI270::read_accel_x: Failed to get sensor data.");
        }
    }
    _accelX = false;
    return data.accelX;
}

float flxDevBMI270::read_accel_y()
{
    if (!_accelY) {
        if (BMI2_OK == getSensorData()) {
            _accelX = true;
            _accelZ = true;
            _gyroX = true;
            _gyroY = true;
            _gyroZ = true;
        }
        else {
            flxLog_E("BMI270::read_accel_y: Failed to get sensor data.");
        }
    }
    _accelY = false;
    return data.accelY;
}

float flxDevBMI270::read_accel_z()
{
    if (!_accelZ) {
        if (BMI2_OK == getSensorData()) {
            _accelY = true;
            _accelX = true;
            _gyroX = true;
            _gyroY = true;
            _gyroZ = true;
        }
        else {
            flxLog_E("BMI270::read_accel_z: Failed to get sensor data.");
        }
    }
    _accelZ = false;
    return data.accelZ;
}

float flxDevBMI270::read_gyro_x()
{
    if (!_gyroX) {
        if (BMI2_OK == getSensorData()) {
            _accelX = true;
            _accelY = true;
            _accelZ = true;
            _gyroY = true;
            _gyroZ = true;
        }
        else {
            flxLog_E("BMI270::read_gyro_x: Failed to get sensor data.");
        }
    }
    _gyroX = false;
    return data.gyroX;
}

float flxDevBMI270::read_gyro_y()
{
    if (!_gyroY) {
        if (BMI2_OK == getSensorData()) {
            _accelX = true;
            _accelY = true;
            _accelZ = true;
            _gyroX = true;
            _gyroZ = true;
        }
        else {
            flxLog_E("BMI270::read_gyro_y: Failed to get sensor data.");
        }
    }
    _gyroY = false;
    return data.gyroY;
}

float flxDevBMI270::read_gyro_z()
{
    if (!_gyroZ) {
        if (BMI2_OK == getSensorData()) {
            _accelX = true;
            _accelY = true;
            _accelZ = true;
            _gyroY = true;
            _gyroX = true;
        }
        else {
            flxLog_E("BMI270::read_gyro_z: Failed to get sensor data.");
        }
    }
    _gyroZ = false;
    return data.gyroZ;
}

float flxDevBMI270::read_temperature()
{
    if (BMI2_OK != getTemperature(_tempC)) {
        flxLog_E("BMI270::read_temperature: Failed to get temperature data.");
    }
    return _tempC;
}

uint flxDevBMI270::read_step_count()
{
    if (!_stepCounterEnabled) {
        flxLog_W("BMI270::read_step_count: Step counting feature not enabled. Enabling...");
        if (!enable_step_counter(true)) {
            flxLog_E("BMI270::read_step_count: Failed to enable step counting feature. Logging last good value.");
        }
    }

    if (_stepCounterEnabled) {
        if (BMI2_OK != getStepCount(_countedSteps)) {
            flxLog_E("BMI270::read_step_count: Failed to get step count. Logging last good value.");
        }
    }

    return ((uint) _countedSteps);
}

/* Write methods for parameters */

void flxDevBMI270::write_reset_step_count()
{
    if(_stepCounterEnabled) {
        if (BMI2_OK != resetStepCount()) {
            flxLog_E("BMI270::write_reset_step_count: Failed to reset step count");
        }
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
    return gyroConfig.cfg.gyro.range;
}

/* Setter methods for properties */

void flxDevBMI270::set_accel_odr(uint8_t dataRate)
{
    if (BMI2_OK != setAccelODR(dataRate)){
        flxLog_E("BMI270::set_accel_odr: Could not set ODR.");
    }
    else {
        accelConfig.cfg.acc.odr = dataRate;
    }
}

void flxDevBMI270::set_accel_power_mode(uint8_t powerMode)
{
    if (BMI2_OK != setAccelPowerMode(powerMode)){
        flxLog_E("BMI270::set_accel_power_mode: Could not set mode.");
    }
    else {
        accelConfig.cfg.acc.filter_perf = powerMode;
    }
}

void flxDevBMI270::set_accel_filter_bandwidth(uint8_t bandwidth)
{
    if (BMI2_OK != setAccelFilterBandwidth(bandwidth)){
        flxLog_E("BMI270::set_accel_filter_bandwidth: Could not set bandwidth.");
    }
    else {
        accelConfig.cfg.acc.bwp = bandwidth;
    }
}

void flxDevBMI270::set_accel_range(uint8_t range)
{
    uint8_t prevRange = accelConfig.cfg.acc.range;

    accelConfig.cfg.acc.range = range;

    if(BMI2_OK != setConfig(accelConfig)) {
        flxLog_E("BMI270::set_accel_range: Could not set range.");
        accelConfig.cfg.acc.range = prevRange;
    }
}

void flxDevBMI270::set_gyro_odr(uint8_t dataRate)
{
    if (BMI2_OK != setGyroODR(dataRate)){
        flxLog_E("BMI270::set_gyro_odr: Could not set ODR.");
    }
    else {
        gyroConfig.cfg.gyr.odr = dataRate;
    }
}

void flxDevBMI270::set_gyro_power_mode_filter(uint8_t powerMode)
{
    if (BMI2_OK != setGyroPowerMode(powerMode, gyroConfig.cfg.gyr.noise_perf)){
        flxLog_E("BMI270::set_gyro_power_mode_filter: Could not set filter mode.");
    }
    else {
        gyroConfig.cfg.gyr.filter_perf = powerMode;
    }
}

void flxDevBMI270::set_gyro_power_mode_noise(uint8_t powerMode)
{
    if (BMI2_OK != setGyroPowerMode(gyroConfig.cfg.gyr.filter_perf, powerMode)){
        flxLog_E("BMI270::set_gyro_power_mode_filter: Could not set filter mode.");
    }
    else {
        gyroConfig.cfg.gyr.noise_perf = powerMode;
    }
}

void flxDevBMI270::set_gyro_filter_bandwidth(uint8_t bandwidth)
{
    if (BMI2_OK != setGyroFilterBandwidth(bandwidth)){
        flxLog_E("BMI270::set_gyro_filter_bandwidth: Could not set bandwidth.");
    }
    else {
        gyroConfig.cfg.gyr.bwp = bandwidth;
    }
}

void flxDevBMI270::set_gyro_range(uint8_t range)
{
    uint8_t prevRange = gyroConfig.cfg.gyr.range;

    gyroConfig.cfg.gyr.range = range;

    if(BMI2_OK != setConfig(gyroConfig)) {
        flxLog_E("BMI270::set_gyro_range: Could not set range.");
        gyroConfig.cfg.gyr.range = prevRange;
    }
}

void flxDevBMI270::enable_step_counter(bool enabled)
{
    if(enabled && !_stepCounterEnabled) {
        if(BMI2_OK != enableFeature(BMI2_STEP_COUNTER)) {
            flxLog_E("BMI270::enable_step_counter: Could not enable step counter.");
        }
        _stepCounterEnabled = true;
    }

    else if(!enabled && _stepCounterEnabled) {
        if(BMI2_OK != disableFeature(BMI2_STEP_COUNTER)) {
            flxLog_E("BMI270::enable_step_counter: Could not disable step counter.");
        }
        _stepCounterEnabled = false;
    }
}
