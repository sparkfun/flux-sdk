/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevVEML6030.h
 *
 *  Spark Device object for the VEML6030 Ambient Light Sensor.
 *
 */

#pragma once

#include "Arduino.h"

#include <memory>

#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"
#include "flxDevice.h"

#define kVEML6030DeviceName "VEML6030"

// The library constructor requires the I2C address, so we use composition
// rather than inheritance and allocate the sensor object in onInitialize().
class flxDevVEML6030 : public flxDeviceI2CType<flxDevVEML6030>
{

  public:
    flxDevVEML6030();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        // No device-ID register; reserved-bit check only → fuzzy confidence so
        // devices with exact ID registers (TMP117, ADS1015) take priority at
        // the shared 0x48 address.
        return flxDevConfidenceFuzzy;
    }

    static const char *getDeviceName()
    {
        return kVEML6030DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    std::unique_ptr<SparkFun_Ambient_Light> _sensor;

    // Output parameter readers
    uint32_t read_ambient_light();
    uint32_t read_white_light();

    // Property getters/setters
    // Gain is stored as an index (0–3) mapped to the float values the library expects.
    uint8_t get_gain();
    void set_gain(uint8_t);
    uint16_t get_integration_time();
    void set_integration_time(uint16_t);

    uint8_t _gain = 0;            // index 0 → 1x gain (power-on default)
    uint16_t _integrationTime = 100; // ms (power-on default)

  public:
    // Gain: 0=x1, 1=x2, 2=x1/8, 3=x1/4
    flxPropertyRWUInt8<flxDevVEML6030, &flxDevVEML6030::get_gain, &flxDevVEML6030::set_gain> gain = {
        0,
        {{"x1", 0}, {"x2", 1}, {"x1/8", 2}, {"x1/4", 3}}};

    // Integration time in milliseconds
    flxPropertyRWUInt16<flxDevVEML6030, &flxDevVEML6030::get_integration_time, &flxDevVEML6030::set_integration_time>
        integrationTime = {100,
                           {{"25ms", 25},
                            {"50ms", 50},
                            {"100ms", 100},
                            {"200ms", 200},
                            {"400ms", 400},
                            {"800ms", 800}}};

    // Lux outputs — both channels already apply the library's compensation formula.
    flxParameterOutUInt32<flxDevVEML6030, &flxDevVEML6030::read_ambient_light> ambientLight;
    flxParameterOutUInt32<flxDevVEML6030, &flxDevVEML6030::read_white_light> whiteLight;
};
