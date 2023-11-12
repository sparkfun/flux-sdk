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
 *  flxDevBMI270.h
 *
 *  Spark Device object for the BMI270 6DoF IMU device.
 *
 *
 *
 */

#pragma once

#include <Arduino.h>

#include "SparkFun_BMI270_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kBMI270DeviceName "BMI270"

//-----------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic library
class flxDevBMI270 : public flxDeviceI2CType<flxDevBMI270>, public BMI270
{
  public:
    flxDevBMI270();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBMI270DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    };
    // This holds the class list of possible addresses/IDs for this object
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

  private:
    // Methods used to get values for our output parameters
    float read_accel_x();
    float read_accel_y();
    float read_accel_z();
    float read_gyro_x();
    float read_gyro_y();
    float read_gyro_z();
    float read_temperature();

    uint read_step_count();

    // Methods used to set values for our input parameters
    void write_reset_step_count();

    // Methods used to get values for our RW properties
    uint8_t get_accel_odr();
    uint8_t get_accel_power_mode();
    uint8_t get_accel_filter_bandwidth();
    uint8_t get_accel_range();
    uint8_t get_gyro_odr();
    uint8_t get_gyro_power_mode_filter();
    uint8_t get_gyro_power_mode_noise();
    uint8_t get_gyro_filter_bandwidth();
    uint8_t get_gyro_range();

    // Methods used to set values for our RW properties
    void set_accel_odr(uint8_t);
    void set_accel_power_mode(uint8_t);
    void set_accel_filter_bandwidth(uint8_t);
    void set_accel_range(uint8_t);
    void set_gyro_odr(uint8_t);
    void set_gyro_power_mode_filter(uint8_t);
    void set_gyro_power_mode_noise(uint8_t);
    void set_gyro_filter_bandwidth(uint8_t);
    void set_gyro_range(uint8_t);

    bool _accelX = false;
    bool _accelY = false;
    bool _accelZ = false;
    bool _gyroX = false;
    bool _gyroY = false;
    bool _gyroZ = false;

    float _tempC = 0.0;

    bmi2_sens_config accelConfig = {.type = BMI2_ACCEL,
                                    .cfg = {.acc = {.odr = BMI2_ACC_ODR_100HZ,
                                                    .bwp = BMI2_ACC_NORMAL_AVG4,
                                                    .filter_perf = BMI2_PERF_OPT_MODE,
                                                    .range = BMI2_ACC_RANGE_8G}}};

    bmi2_sens_config gyroConfig = {.type = BMI2_GYRO,
                                   .cfg = {.gyr = {.odr = BMI2_GYR_ODR_200HZ,
                                                   .bwp = BMI2_GYR_NORMAL_MODE,
                                                   .filter_perf = BMI2_PERF_OPT_MODE,
                                                   .range = BMI2_GYR_RANGE_2000,
                                                   .noise_perf = BMI2_PERF_OPT_MODE}}};

    bool enable_step_counter(bool);
    bool _stepCounterEnabled = false;
    uint32_t _countedSteps = 0;

    bool _sensorInitialized = false;

  public:
    // Define the sensor's Accelerometer Output Data Rate (ODR) in Hz.
    // Default value is BMI2_ACC_ODR_100HZ.
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_accel_odr, &flxDevBMI270::set_gyro_odr> accelDataRate = {
        BMI2_ACC_ODR_100HZ,
        {{"0.78 Hz", BMI2_ACC_ODR_0_78HZ},
         {"1.56 Hz", BMI2_ACC_ODR_1_56HZ},
         {"3.12 Hz", BMI2_ACC_ODR_3_12HZ},
         {"6.25 Hz", BMI2_ACC_ODR_6_25HZ},
         {"12.5 Hz", BMI2_ACC_ODR_12_5HZ},
         {"25 Hz", BMI2_ACC_ODR_25HZ},
         {"50 Hz", BMI2_ACC_ODR_50HZ},
         {"100 Hz", BMI2_ACC_ODR_100HZ},
         {"200 Hz", BMI2_ACC_ODR_200HZ},
         {"400 Hz", BMI2_ACC_ODR_400HZ},
         {"800 Hz", BMI2_ACC_ODR_800HZ},
         {"1600 Hz", BMI2_ACC_ODR_1600HZ}}};

    // Define the sensor's Accelerometer Power Mode.
    // Default value is BMI2_PERF_OPT_MODE.
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_accel_power_mode, &flxDevBMI270::set_accel_power_mode>
        accelPowerMode = {BMI2_PERF_OPT_MODE,
                          {{"Performance Mode", BMI2_PERF_OPT_MODE}, {"Power Saving Mode", BMI2_POWER_OPT_MODE}}};

    // Define the sensor's Accelerometer Filter Bandwidth. Power mode affects which it does for each type.
    // Default Value is BMI2_ACC_NORMAL_AVG4
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_accel_filter_bandwidth,
                       &flxDevBMI270::set_accel_filter_bandwidth>
        accelFilterBW = {BMI2_ACC_NORMAL_AVG4,
                         {{"OSR4 or No Averaging", BMI2_ACC_OSR4_AVG1},
                          {"OSR2 or Average 2 Samples", BMI2_ACC_OSR2_AVG2},
                          {"Normal or Average 4 Samples", BMI2_ACC_NORMAL_AVG4},
                          {"CIC or Average 8 Samples", BMI2_ACC_CIC_AVG8},
                          {"Reserved or Average 16 Samples", BMI2_ACC_RES_AVG16},
                          {"Reserved or Average 32 Samples", BMI2_ACC_RES_AVG32},
                          {"Reserved or Average 64 Samples", BMI2_ACC_RES_AVG64},
                          {"Reserved or Average 128 Samples", BMI2_ACC_RES_AVG128}}};

    // Define the sensor's Accelerometer g-range in g's.
    // Default value is BMI2_ACC_RANGE_8G
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_accel_range, &flxDevBMI270::set_accel_range> accelRange = {
        BMI2_ACC_RANGE_8G,
        {{"+/- 2 g", BMI2_ACC_RANGE_2G},
         {"+/- 4 g", BMI2_ACC_RANGE_4G},
         {"+/- 8 g", BMI2_ACC_RANGE_8G},
         {"+/- 16 g", BMI2_ACC_RANGE_16G}}};

    // Define the sensor's Gyroscope Output Data Rate (ODR) in Hz.
    // Default Value is BMI2_GYR_ODR_200HZ
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_gyro_odr, &flxDevBMI270::set_gyro_odr> gyroDataRate = {
        BMI2_GYR_ODR_200HZ,
        {{"25 Hz", BMI2_GYR_ODR_25HZ},
         {"50 Hz", BMI2_GYR_ODR_50HZ},
         {"100 Hz", BMI2_GYR_ODR_100HZ},
         {"200 Hz", BMI2_GYR_ODR_200HZ},
         {"400 Hz", BMI2_GYR_ODR_400HZ},
         {"800 Hz", BMI2_GYR_ODR_800HZ},
         {"1600 Hz", BMI2_GYR_ODR_1600HZ},
         {"3200 Hz", BMI2_GYR_ODR_3200HZ}}};

    // Define the sensor's Gyroscope Filter Power Mode.
    // Default Value is BMI2_PERF_OPT_MODE
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_gyro_power_mode_filter,
                       &flxDevBMI270::set_gyro_power_mode_filter>
        gyroFilterPowerMode = {BMI2_PERF_OPT_MODE,
                               {{"Performance Mode", BMI2_PERF_OPT_MODE}, {"Power Saving Mode", BMI2_POWER_OPT_MODE}}};

    // Define the sensor's Gyroscope Noise Power Mode.
    // Default Value is BMI2_PERF_OPT_MODE
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_gyro_power_mode_noise, &flxDevBMI270::set_gyro_power_mode_noise>
        gyroNoisePowerMode = {BMI2_PERF_OPT_MODE,
                              {{"Performance Mode", BMI2_PERF_OPT_MODE}, {"Power Saving Mode", BMI2_POWER_OPT_MODE}}};

    // Define the sensor's Gyroscope Filter Bandwidth.
    // Default Value is BMI2_GYR_NORMAL_MODE
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_gyro_filter_bandwidth, &flxDevBMI270::set_gyro_filter_bandwidth>
        gyroFilterBW = {BMI2_GYR_NORMAL_MODE,
                        {{"OSR4", BMI2_GYR_OSR4_MODE},
                         {"OSR2", BMI2_GYR_OSR2_MODE},
                         {"Normal", BMI2_GYR_NORMAL_MODE},
                         {"CIC", BMI2_GYR_CIC_MODE}}};

    // Define the sensor's Gyroscope limit range in deg/s.
    // Default Value is BMI2_GYR_RANGE_2000
    flxPropertyRWUint8<flxDevBMI270, &flxDevBMI270::get_gyro_range, &flxDevBMI270::set_gyro_range> gyroRange = {
        BMI2_GYR_RANGE_2000,
        {{"+/- 2000 deg/s", BMI2_GYR_RANGE_2000},
         {"+/- 1000 deg/s", BMI2_GYR_RANGE_1000},
         {"+/- 500 deg/s", BMI2_GYR_RANGE_500},
         {"+/- 250 deg/s", BMI2_GYR_RANGE_250},
         {"+/- 125 deg/s", BMI2_GYR_RANGE_125}}};

    // Define our output parameters
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_accel_x> accelX;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_accel_y> accelY;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_accel_z> accelZ;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_gyro_x> gyroX;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_gyro_y> gyroY;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_gyro_z> gyroZ;
    flxParameterOutFloat<flxDevBMI270, &flxDevBMI270::read_temperature> temperature;

    flxParameterOutUint<flxDevBMI270, &flxDevBMI270::read_step_count> stepCount;

    // Define our input parameters
    flxParameterInVoid<flxDevBMI270, &flxDevBMI270::write_reset_step_count> resetStepCount;
};
