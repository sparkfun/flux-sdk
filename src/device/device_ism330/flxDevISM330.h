/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 * QwiicDevISM330.h
 *
 *  Device object for the ISM330 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_ISM330DHCX.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kISM330DeviceName "ISM330"

//----------------------------------------------------------------------------------------------------------
// Define a base framework device class. Then subclass from this for I2C and SPI version
// of the device driver

class flxDevISM330Base : public flxDevice, public QwDevISM330DHCX
{

  public:
    flxDevISM330Base();

  private:
    // methods used to get values for our output parameters
    float read_accel_x();
    float read_accel_y();
    float read_accel_z();
    float read_gyro_x();
    float read_gyro_y();
    float read_gyro_z();
    float read_temperature();

    // methods used to get values for our RW properties
    uint8_t get_accel_data_rate();
    void set_accel_data_rate(uint8_t);
    uint8_t get_accel_full_scale();
    void set_accel_full_scale(uint8_t);
    uint8_t get_gyro_data_rate();
    void set_gyro_data_rate(uint8_t);
    uint8_t get_gyro_full_scale();
    void set_gyro_full_scale(uint8_t);
    uint8_t get_accel_filter_lp2();
    void set_accel_filter_lp2(uint8_t);
    uint8_t get_gyro_filter_lp1();
    void set_gyro_filter_lp1(uint8_t);
    uint8_t get_accel_slope_filter();
    void set_accel_slope_filter(uint8_t);
    uint8_t get_gyro_lp1_bandwidth();
    void set_gyro_lp1_bandwidth(uint8_t);

    // Flags to prevent getAccel being called multiple times
    bool _accelX = false;
    bool _accelY = false;
    bool _accelZ = false;

    // Flags to prevent getGyro being called multiple times
    bool _gyroX = false;
    bool _gyroY = false;
    bool _gyroZ = false;

    sfe_ism_data_t _accelData;
    sfe_ism_data_t _gyroData;

    uint8_t _accel_data_rate = ISM_XL_ODR_104Hz;
    uint8_t _accel_full_scale = ISM_4g;
    uint8_t _gyro_data_rate = ISM_GY_ODR_104Hz;
    uint8_t _gyro_full_scale = ISM_500dps;
    bool _accel_filter_lp2 = true;
    bool _gyro_filter_lp1 = true;
    uint8_t _accel_slope_filter = ISM_LP_ODR_DIV_100;
    uint8_t _gyro_lp1_bandwidth = ISM_MEDIUM;

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_accel_x> accelX;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_accel_y> accelY;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_accel_z> accelZ;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_gyro_x> gyroX;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_gyro_y> gyroY;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_gyro_z> gyroZ;
    flxParameterOutFloat<flxDevISM330Base, &flxDevISM330Base::read_temperature> temperature;

    // Define our read-write properties
    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_accel_data_rate, &flxDevISM330Base::set_accel_data_rate>
        accelDataRate = {ISM_XL_ODR_104Hz,
                         {{"Off", ISM_XL_ODR_OFF},
                          {"12.5 Hz", ISM_XL_ODR_12Hz5},
                          {"26 Hz", ISM_XL_ODR_26Hz},
                          {"52 Hz", ISM_XL_ODR_52Hz},
                          {"104 Hz", ISM_XL_ODR_104Hz},
                          {"208 Hz", ISM_XL_ODR_208Hz},
                          {"416 Hz", ISM_XL_ODR_416Hz},
                          {"833 Hz", ISM_XL_ODR_833Hz},
                          {"1666 Hz", ISM_XL_ODR_1666Hz},
                          {"3332 Hz", ISM_XL_ODR_3332Hz},
                          {"6667 Hz", ISM_XL_ODR_6667Hz},
                          {"1.6 Hz", ISM_XL_ODR_1Hz6}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_accel_full_scale,
                       &flxDevISM330Base::set_accel_full_scale>
        accelFullScale = {ISM_4g, {{"2 g", ISM_2g}, {"16 g", ISM_16g}, {"4 g", ISM_4g}, {"8 g", ISM_8g}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_gyro_data_rate, &flxDevISM330Base::set_gyro_data_rate>
        gyroDataRate = {ISM_GY_ODR_104Hz,
                        {{"Off", ISM_GY_ODR_OFF},
                         {"12 Hz", ISM_GY_ODR_12Hz},
                         {"26 Hz", ISM_GY_ODR_26Hz},
                         {"52 Hz", ISM_GY_ODR_52Hz},
                         {"104 Hz", ISM_GY_ODR_104Hz},
                         {"208 Hz", ISM_GY_ODR_208Hz},
                         {"416 Hz", ISM_GY_ODR_416Hz},
                         {"833 Hz", ISM_GY_ODR_833Hz},
                         {"1666 Hz", ISM_GY_ODR_1666Hz},
                         {"3332 Hz", ISM_GY_ODR_3332Hz},
                         {"6667 Hz", ISM_GY_ODR_6667Hz}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_gyro_full_scale, &flxDevISM330Base::set_gyro_full_scale>
        gyroFullScale = {ISM_500dps,
                         {{"125 dps", ISM_125dps},
                          {"250 dps", ISM_250dps},
                          {"500 dps", ISM_500dps},
                          {"1000 dps", ISM_1000dps},
                          {"2000 dps", ISM_2000dps},
                          {"4000 dps", ISM_4000dps}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_accel_filter_lp2,
                       &flxDevISM330Base::set_accel_filter_lp2>
        accelFilterLP2 = {1, {{"Enabled", 1}, {"Disabled", 0}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_gyro_filter_lp1, &flxDevISM330Base::set_gyro_filter_lp1>
        gyroFilterLP1 = {1, {{"Enabled", 1}, {"Disabled", 0}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_accel_slope_filter,
                       &flxDevISM330Base::set_accel_slope_filter>
        accelSlopeFilter = {ISM_LP_ODR_DIV_100,
                            {{"ODR/4", 0},
                             {"ODR/10", ISM_LP_ODR_DIV_10},
                             {"ODR/20", ISM_LP_ODR_DIV_20},
                             {"ODR/45", ISM_LP_ODR_DIV_45},
                             {"ODR/100", ISM_LP_ODR_DIV_100},
                             {"ODR/200", ISM_LP_ODR_DIV_200},
                             {"ODR/400", ISM_LP_ODR_DIV_400},
                             {"ODR/800", ISM_LP_ODR_DIV_800}}};

    flxPropertyRWUInt8<flxDevISM330Base, &flxDevISM330Base::get_gyro_lp1_bandwidth,
                       &flxDevISM330Base::set_gyro_lp1_bandwidth>
        gyroLP1Bandwidth = {ISM_MEDIUM,
                            {{"Ultra Light", ISM_ULTRA_LIGHT},
                             {"Very Light", ISM_VERY_LIGHT},
                             {"Light", ISM_LIGHT},
                             {"Medium", ISM_MEDIUM},
                             {"Strong", ISM_STRONG},
                             {"Very Strong", ISM_VERY_STRONG},
                             {"Aggressive", ISM_AGGRESSIVE},
                             {"Extreme", ISM_XTREME}}};

  protected:
    bool onInitialize(void);
};

//----------------------------------------------------------------------------------------------------------
// I2C version of the driver
//----------------------------------------------------------------------------------------------------------
//
// Define our I2C version of the class. NOTE: We add our base class to the template call

class flxDevISM330 : public flxDeviceI2CType<flxDevISM330, flxDevISM330Base>
{

  public:
    flxDevISM330() {};

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kISM330DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

  private:
    // The low-level driver I2C bus class
    sfe_ISM330DHCX::QwI2C _i2cBus;
};

//----------------------------------------------------------------------------------------------------------
// SPI version of the driver
//----------------------------------------------------------------------------------------------------------
//
// Define our SPI version of the class. NOTE: We add our base class to the template call

class flxDevISM330_SPI : public flxDeviceSPIType<flxDevISM330_SPI, flxDevISM330Base>
{

  public:
    flxDevISM330_SPI() {};

    // Method called to initialize the class
    bool onInitialize(SPIClass &);

  private:
    // The low-level driver SPI bus class
    sfe_ISM330DHCX::SfeSPI _spiBus;
};