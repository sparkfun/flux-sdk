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
 *  flxDevLidarLiteV4.h
 *
 *  Spark Device object for the LIDAR-Lite v4 LED device.
 *
 */

#pragma once

#include "Arduino.h"

#include "LIDARLite_v4LED.h"
#include "flxDevice.h"

#define kLidarLiteV4DeviceName "LidarLiteV4"

class flxDevLidarLiteV4 : public flxDeviceI2CType<flxDevLidarLiteV4>, public LIDARLite_v4LED
{

  public:
    flxDevLidarLiteV4();

    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidencePing;
    }

    static const char *getDeviceName()
    {
        return kLidarLiteV4DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    uint16_t read_distance();
    int8_t read_board_temperature();
    int8_t read_soc_temperature();

    uint8_t get_configuration();
    void set_configuration(uint8_t);

    uint8_t _configuration = 0;

  public:
    flxParameterOutUInt16<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_distance> distance;
    flxParameterOutInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_board_temperature> boardTemperature;
    flxParameterOutInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_soc_temperature> socTemperature;

    flxPropertyRWUInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::get_configuration, &flxDevLidarLiteV4::set_configuration>
        configuration = {0,
                         {{"Max Range", 0},
                          {"Balanced", 1},
                          {"Short Range, High Speed", 2},
                          {"Mid Range, Higher Speed", 3},
                          {"Max Range, Higher Speed", 4},
                          {"Very Short Range, Highest Speed", 5}}};
};
