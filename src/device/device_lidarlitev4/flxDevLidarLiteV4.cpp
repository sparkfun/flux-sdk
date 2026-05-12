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
 *  flxDevLidarLiteV4.cpp
 *
 *  Spark Device object for the LIDAR-Lite v4 LED device.
 *
 */

#include "Arduino.h"

#include "flxDevLidarLiteV4.h"

#define kLidarLiteV4AddressDefault 0x62

uint8_t flxDevLidarLiteV4::defaultDeviceAddress[] = {kLidarLiteV4AddressDefault, kSparkDeviceAddressNull};

flxRegisterDevice(flxDevLidarLiteV4);

flxDevLidarLiteV4::flxDevLidarLiteV4()
{
    setName(getDeviceName());
    setDescription("Garmin LIDAR-Lite v4 LED Distance Sensor");

    flxRegister(distance, "Distance (cm)", "Measured distance in centimeters", kParamValueDistance);
    flxRegister(boardTemperature, "Board Temperature (C)", "Board temperature in degrees Celsius", kParamValueTempC);
    flxRegister(socTemperature, "SoC Temperature (C)", "SoC temperature in degrees Celsius", kParamValueTempC);

    flxRegister(configuration, "Configuration", "Measurement configuration preset (0=Max Range, 5=Very Short/Fastest)");
}

bool flxDevLidarLiteV4::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    return i2cDriver.ping(address);
}

bool flxDevLidarLiteV4::onInitialize(TwoWire &wirePort)
{
    bool status = LIDARLite_v4LED::begin(address(), wirePort);
    if (status)
        LIDARLite_v4LED::configure(_configuration);
    return status;
}

uint16_t flxDevLidarLiteV4::read_distance()
{
    return LIDARLite_v4LED::getDistance();
}

int8_t flxDevLidarLiteV4::read_board_temperature()
{
    return (int8_t)LIDARLite_v4LED::getBoardTemp();
}

int8_t flxDevLidarLiteV4::read_soc_temperature()
{
    return (int8_t)LIDARLite_v4LED::getSOCTemp();
}

uint8_t flxDevLidarLiteV4::get_configuration()
{
    return _configuration;
}

void flxDevLidarLiteV4::set_configuration(uint8_t config)
{
    _configuration = config;
    if (isInitialized())
        LIDARLite_v4LED::configure(_configuration);
}
