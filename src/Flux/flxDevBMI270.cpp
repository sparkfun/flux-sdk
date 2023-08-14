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

#define kBMI270ChipID 0x24

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
    flxRegister(accelDataRate, "Accelerometer ODR", "Output Data Rate for the Accelerometer in Hz.");
    flxRegister(accelPowerMode, "Accelerometer Power Mode", "Power mode for the Accelerometer.");
    flxRegister(accelFilterBW, "Accelerometer Filter Bandwidth", "Filter BW for the accelerometer in order of Performance or Power Saving Mode.");
    flxRegister(gyroDataRate, "Gyroscope ODR", "Output Data Rate for the Gyroscope in Hz.");
    flxRegister(gyroFilterPowerMode, "Gyroscope Filter Power Mode", "Power Mode for the Gyroscope Filter.");
    flxRegister(gyroNoisePowerMode, "Gyroscope Noise Power Mode", "Low noise mode for precision yaw rate sensing.");
    flxRegister(gyroFilterBW, "Gyroscope Filter Bandwidth", "Filter BW for the gyroscope.");

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
    flxRegister(resetStepCount, "Reset Step Count", "Write 1 to reset the number of steps counted.");
}

//-----------------------------------------------------------------------------
// Static method used to determine if device is connected before creating the 
// object (if creating dynamically)
bool flxDevBMI270::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;
    
    // TODO: Add in a way to verify that you're talking to the right device, such as verifying serial number, product ID, etc.
    uint8_t readValue = 0;

    return (readValue == kBMI270ChipID);
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
    // TODO: Any kind of initialization and verification that it's up and running, etc. 
    return TEMPLATECLASS::begin(wirePort); /* REPLACE ME */
}

/* Read methods for parameters */

float flxDevBMI270::read_accel_x()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_accel_y()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_accel_z()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_gyro_x()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_gyro_y()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_gyro_z()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_temperature()
{
    // TODO: Get that good stuff from the sensor
}

float flxDevBMI270::read_step_count()
{
    // TODO: Get that good stuff from the sensor
}

/* Write methods for parameters */

void flxDevBMI270::write_reset_step_count(bool resetSteps)
{
    // TODO: Write some value to the sensor
}

/* Getter methods for properties */

uint8_t flxDevBMI270::get_accel_odr()
{
    // TODO: Return accelerometer ODR
}

uint8_t flxDevBMI270::get_accel_power_mode()
{
    // TODO: Return accelerometer Power Mode
}

uint8_t flxDevBMI270::get_accel_filter_bandwidth()
{
    // TODO: Return accelerometer Filter Bandwidth
}

uint8_t flxDevBMI270::get_gyro_odr()
{
    // TODO: Return gyro ODR
}

uint8_t flxDevBMI270::get_gyro_power_mode_filter()
{
    // TODO: Return gyro filter power mode
}

uint8_t flxDevBMI270::get_gyro_power_mode_noise()
{
    // TODO: Return gyro noise power mode
}

uint8_t flxDevBMI270::get_gyro_filter_bandwidth()
{
    // TODO: Return gyro filter bandwidth
}

/* Setter methods for properties */

void flxDevBMI270::set_accel_odr(uint8_t dataRate)
{
    // TODO: Set accelerometer ODR
}

void flxDevBMI270::set_accel_power_mode(uint8_t powerMode)
{
    // TODO: Set accelerometer Power Mode
}

void flxDevBMI270::set_accel_filter_bandwidth(uint8_t bandwidth)
{
    // TODO: Set accelerometer Filter Bandwidth
}

void flxDevBMI270::set_gyro_odr(uint8_t dataRate)
{
    // TODO: Set gyro ODR
}

void flxDevBMI270::set_gyro_power_mode_filter(uint8_t powerMode)
{
    // TODO: Set gyro filter power mode
}

void flxDevBMI270::set_gyro_power_mode_noise(uint8_t powerMode)
{
    // TODO: Set gyro noise power mode
}

void flxDevBMI270::set_gyro_filter_bandwidth(uint8_t bandwidth)
{
    // TODO: Set gyro filter bandwidth
}

void flxDevBMI270::enable_step_counter(bool enabled)
{
    // TODO: Enable the step counter feature
}
